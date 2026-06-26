/*
 * Copyright (C) 2024-2025 Advanced Micro Devices, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <Common/FileUtils.hpp>
#include <OpenCL/CLUtils.hpp>


struct Matrix {
	std::vector<float> data;
	size_t rows, cols;

	Matrix(size_t r, size_t c) : data(r * c, 0.0f), rows(r), cols(c) {}

	float & operator()(size_t i, size_t j) { return data[i * cols + j]; }
	const float & operator()(size_t i, size_t j) const { return data[i * cols + j]; }

	size_t size() const { return rows * cols; }
	float * data_ptr() { return data.data(); }
};

static Matrix transpose(const Matrix & m)
{
	Matrix result(m.cols, m.rows);
	for (size_t i = 0; i < m.rows; ++i) {
		for (size_t j = 0; j < m.cols; ++j) {
			result(j, i) = m(i, j);
		}
	}
	return result;
}

static bool notEquals(const Matrix & a, const Matrix & b, float epsilon)
{
	if (a.rows != b.rows || a.cols != b.cols) return true;
	for (size_t i = 0; i < a.rows; ++i) {
		for (size_t j = 0; j < a.cols; ++j) {
			if (std::abs(a(i, j) - b(i, j)) > epsilon) return true;
		}
	}
	return false;
}

static Matrix operator*(const Matrix & a, const Matrix & b)
{
	if (a.cols != b.rows) throw std::runtime_error("Matrix dimensions mismatch for multiplication");
	Matrix result(a.rows, b.cols);
	for (size_t i = 0; i < a.rows; ++i) {
		for (size_t j = 0; j < b.cols; ++j) {
			float sum = 0.0f;
			for (size_t k = 0; k < a.cols; ++k) {
				sum += a(i, k) * b(k, j);
			}
			result(i, j) = sum;
		}
	}
	return result;
}

class Timer {
	using Clock = std::chrono::high_resolution_clock;
	Clock::time_point start;

public:
	Timer() : start(Clock::now()) {}
	void Reset() { start = Clock::now(); }
	float getElapsedMilliSecsF() const {
		return std::chrono::duration<float, std::milli>(Clock::now() - start).count();
	}
};

static void printHelp()
{
	std::cout << "\nSupported command line options:\n"
		<< "    -h                    : Displays this help message\n"
		<< "    -v                    : Verbose mode. Adds more timing information\n"
		<< "    -va                   : Validate mode: Uses CPU-side matrix multiplication to validate results\n"
		<< "    -csv [filename]       : Save results to a CSV file (default: results.csv)\n"
		<< "    -kernel [kernelname]  : Specify a kernel to run. If blank, runs all kernels. Supported kernels:\n"
		<< "                            matmul_naive          : Naive matrix multiplication\n"
		<< "                            matmul_transposedA    : Naive, with left matrix (A) transposed\n"
		<< "                            matmul_transposedB    : Naive, with right matrix (B) transposed\n"
		<< "                            matmul_transposedC    : Naive, with result matrix (C) transposed\n"
		<< "                            matmul_transposedAC   : Naive, with A and C transposed\n"
		<< "                            matmul_transposedBC   : Naive, with B and C transposed\n"
		<< "                            matmul_linearwg_AT    : Linear workgroup, A transposed\n"
		<< "                            matmul_linearwg_BT    : Linear workgroup, B transposed\n"
		<< "                            matmul_linearwg_vec4  : Linear workgroup, vec4 inputs\n"
		<< "                            matmul_linearwg_vec4_local : Linear workgroup, vec4 with local memory\n"
		<< "                            matmul_tile_square    : Tiled algorithm with square tiles\n"
		<< "                            matmul_tile_rect      : Tiled algorithm with rectangular tiles\n"
		<< "                            matmul_tile_rect_vec4 : Tiled algorithm with vec4 and rectangular tiles\n"
		<< "                            matmul_optimized      : Optimized tiled algorithm with vec4\n"
		<< "    -m -n -p              : Matrix dimensions (M, N, P). Default: 512,1536,1024\n"
		<< "    -wg_square_side       : Workgroup side length for square workgroup. Default: 16\n"
		<< "    -wg_linear_size       : Linear workgroup size. Default: 32\n"
		<< "    -wg_rect_width        : Workgroup width for rectangular workgroup. Default: 8\n"
		<< "    -wg_rect_height       : Workgroup height for rectangular workgroup. Default: 4\n"
		<< "    -tile_square          : Square tile size. Default: 8\n"
		<< "    -tile_rect_m          : Tile M dimension. Default: 8\n"
		<< "    -tile_rect_n          : Tile N dimension. Default: 64\n"
		<< "    -tile_rect_p          : Tile P dimension. Default: 4\n";
}

struct KernelResult {
	std::string name;
	std::string note;
	float time_ms;
	size_t workgroup_size_x;
	size_t workgroup_size_y;
	bool validated;
	double gflops;
};

struct KernelParams {
	Timer mytime;
	cl_program program;
	cl_command_queue queue;
	size_t globalSize[2];
	size_t localSize[2];
};

static std::vector<KernelResult> results;
static bool VERBOSE = false;
static bool VALIDATE = false;

static void validate_result(const Matrix & a, const Matrix & b, bool & validated)
{
	validated = !notEquals(a, b, 1.0f);
	std::cout << (validated ? "*** SUCCESSFUL ***" : " +++ FAILED +++ ");
}

static void execute_kernel(KernelParams & k, const char * kernelName,
		cl_mem memA, cl_mem memB, cl_mem memC,
		Matrix & C, const Matrix & D, const std::string & note,
		int M, int N, int P, bool is_optimized = false)
{
	std::cout << "\n===== EXECUTION:  " << std::left << std::setw(30) << kernelName
			<< std::setw(10) << note << std::flush;

	if (VERBOSE) {
		std::cout << "\n==> Creating kernels                          - Time: "
				<< k.mytime.getElapsedMilliSecsF() << std::flush;
		k.mytime.Reset();
	}

	const ocl::Kernel kernel = [&] () {
		cl_int err;
		const cl_kernel kern = clCreateKernel(k.program, kernelName, &err);
		if (err != CL_SUCCESS) throw std::runtime_error("Failed to create kernel: " + std::string(kernelName));
		return ocl::Kernel::adapt({}, kern);
	}();

	if (VERBOSE) {
		std::cout << "(" << k.mytime.getElapsedMilliSecsF() << "ms)\n"
				<< "==> Setting up kernel arguments               - Time: "
				<< k.mytime.getElapsedMilliSecsF() << std::flush;
		k.mytime.Reset();
	}

	kernel.setArg<cl_mem>(0, memA);
	kernel.setArg<cl_mem>(1, memB);
	kernel.setArg<cl_mem>(2, memC);
	if (is_optimized) {
		kernel.setArg<cl_int>(3, M);
		kernel.setArg<cl_int>(4, N);
		kernel.setArg<cl_int>(5, P);
	}

	if (VERBOSE) {
		std::cout << "(" << k.mytime.getElapsedMilliSecsF() << "ms)\n"
				<< "==> Executing                                 - Time: "
				<< k.mytime.getElapsedMilliSecsF() << std::flush;
	}
	k.mytime.Reset();

	cl_event rawEvent;
	CHECKCL(clEnqueueNDRangeKernel(k.queue, kernel,
			k.globalSize[1] == 0 ? 1 : 2, nullptr,
			k.globalSize, k.localSize, 0, nullptr, &rawEvent))
	const ocl::Event kernelEvent = ocl::Event::adapt({}, rawEvent);
	(void)kernelEvent;

	CHECKCL(clWaitForEvents(1, &rawEvent))
	float time_ms = k.mytime.getElapsedMilliSecsF();
	double gflops = (2.0 * M * N * P) / (time_ms * 1e6);
	std::cout << "(" << std::fixed << std::setprecision(2) << std::setw(7)
			<< time_ms << "ms, " << gflops << " GFLOPS)" << std::flush;

	if (VERBOSE) {
		std::cout << "\n==> Reading back results                      - Time: "
				<< k.mytime.getElapsedMilliSecsF() << std::flush;
	}

	KernelResult result;
	result.name = kernelName;
	result.note = note;
	result.time_ms = time_ms;
	result.workgroup_size_x = k.localSize[0];
	result.workgroup_size_y = k.localSize[1];
	result.gflops = gflops;
	result.validated = true;

	k.mytime.Reset();

	CHECKCL(clEnqueueReadBuffer(k.queue, memC, CL_TRUE, 0, C.size() * sizeof(float),
			C.data_ptr(), 0, nullptr, nullptr))

	if (VERBOSE) {
		std::cout << "(" << k.mytime.getElapsedMilliSecsF() << "ms)\n";
	}

	if (VALIDATE) {
		if (VERBOSE) std::cout << "==> Validating results:   " << std::flush;
		validate_result(C, D, result.validated);
		if (VERBOSE) std::cout << "\n";
	}

	results.push_back(result);
}

static void try_catch_execute_kernel(KernelParams & k, const char * kernelName,
		cl_mem memA, cl_mem memB, cl_mem memC,
		Matrix & C, const Matrix & D, const std::string & note,
		int M, int N, int P, bool is_optimized = false)
{
	try {
		execute_kernel(k, kernelName, memA, memB, memC, C, D, note, M, N, P, is_optimized);
	} catch (const std::exception & e) {
		std::cout << "Error encountered: " << e.what() << "\n";
		KernelResult result;
		result.name = kernelName;
		result.note = note;
		result.time_ms = 0.0f;
		result.workgroup_size_x = k.localSize[0];
		result.workgroup_size_y = k.localSize[1];
		result.gflops = 0.0;
		result.validated = false;
		results.push_back(result);
	} catch (...) {
		std::cout << "Execution encountered an unknown error\n";
		KernelResult result;
		result.name = kernelName;
		result.note = note;
		result.time_ms = 0.0f;
		result.workgroup_size_x = k.localSize[0];
		result.workgroup_size_y = k.localSize[1];
		result.gflops = 0.0;
		result.validated = false;
		results.push_back(result);
	}
}

static bool assert_param(const std::string & message, bool result)
{
	if (!result) std::cout << "Input error: " << message << "\n";
	return result;
}

static int lcm(int a, int b)
{
	return std::abs(a * b) / std::__gcd(a, b);
}

static void print_results(int M, int N, int P, const std::string & csv_file)
{
	std::cout << "\n\n=== Performance Summary ===\n";
	std::cout << std::left << std::setw(30) << "Kernel Name"
			<< std::setw(15) << "Tile Sizes"
			<< std::setw(15) << "Workgroup Size"
			<< std::setw(10) << "Time (ms)"
			<< std::setw(12) << "GFLOPS";
	if (VALIDATE) std::cout << std::setw(12) << "Validated";
	std::cout << "\n" << std::string(94 + (VALIDATE ? 12 : 0), '-') << "\n";

	std::vector<KernelResult> tiled_results;
	std::vector<KernelResult> other_results;
	for (const auto & r : results) {
		if (r.name == "matmul_tile_rect_vec4" || r.name == "matmul_optimized") tiled_results.push_back(r);
		else other_results.push_back(r);
	}

	for (const auto & r : other_results) {
		std::cout << std::left << std::setw(30) << r.name
				<< std::setw(15) << r.note
				<< std::setw(15) << (std::to_string(r.workgroup_size_x) + "x" + std::to_string(r.workgroup_size_y))
				<< std::fixed << std::setprecision(2) << std::setw(10) << r.time_ms
				<< std::setw(12) << r.gflops;
		if (VALIDATE) std::cout << std::setw(12) << (r.validated ? "Yes" : "No");
		std::cout << "\n";
	}

	if (!tiled_results.empty()) {
		std::cout << "\nTiled Kernels (matmul_tile_rect_vec4, matmul_optimized):\n";
		for (const auto & r : tiled_results) {
			std::cout << std::left << std::setw(30) << r.name
					<< std::setw(15) << r.note
					<< std::setw(15) << (std::to_string(r.workgroup_size_x) + "x" + std::to_string(r.workgroup_size_y))
					<< std::fixed << std::setprecision(2) << std::setw(10) << r.time_ms
					<< std::setw(12) << r.gflops;
			if (VALIDATE) std::cout << std::setw(12) << (r.validated ? "Yes" : "No");
			std::cout << "\n";
		}
	}

	if (!results.empty()) {
		float min_time = results[0].time_ms;
		float max_time = results[0].time_ms;
		float sum_time = 0.0f;
		int count = 0;
		float min_tiled_time = 0.0f;
		float max_tiled_time = 0.0f;
		float sum_tiled_time = 0.0f;
		int tiled_count = 0;

		for (const auto & r : results) {
			if (r.time_ms > 0) {
				min_time = std::min(min_time, r.time_ms);
				max_time = std::max(max_time, r.time_ms);
				sum_time += r.time_ms;
				count++;
				if (r.name == "matmul_tile_rect_vec4" || r.name == "matmul_optimized") {
					if (tiled_count == 0) {
						min_tiled_time = r.time_ms;
						max_tiled_time = r.time_ms;
					} else {
						min_tiled_time = std::min(min_tiled_time, r.time_ms);
						max_tiled_time = std::max(max_tiled_time, r.time_ms);
					}
					sum_tiled_time += r.time_ms;
					tiled_count++;
				}
			}
		}

		std::cout << "\n=== Statistics ===\n";
		std::cout << "All Kernels:\n";
		std::cout << "  Min Time: " << std::fixed << std::setprecision(2) << min_time << " ms\n";
		std::cout << "  Max Time: " << max_time << " ms\n";
		std::cout << "  Avg Time: " << (count > 0 ? sum_time / count : 0.0f) << " ms\n";
		if (tiled_count > 0) {
			std::cout << "Tiled Kernels (matmul_tile_rect_vec4, matmul_optimized):\n";
			std::cout << "  Min Time: " << min_tiled_time << " ms\n";
			std::cout << "  Max Time: " << max_tiled_time << " ms\n";
			std::cout << "  Avg Time: " << (sum_tiled_time / tiled_count) << " ms\n";
		}
	}

	if (!csv_file.empty()) {
		std::ofstream csv(csv_file);
		if (!csv.is_open()) {
			std::cerr << "Failed to open CSV file: " << csv_file << "\n";
			return;
		}
		csv << "Kernel Name,Tile Sizes,Workgroup Size,Time (ms),GFLOPS";
		if (VALIDATE) csv << ",Validated";
		csv << "\n";
		for (const auto & r : results) {
			csv << r.name << "," << r.note << "," << r.workgroup_size_x << "x" << r.workgroup_size_y
					<< "," << std::fixed << std::setprecision(2) << r.time_ms << "," << r.gflops;
			if (VALIDATE) csv << "," << (r.validated ? "Yes" : "No");
			csv << "\n";
		}
		csv.close();
		std::cout << "\nResults saved to " << csv_file << "\n";
	}
}

/*
 * Build an OpenCL program from the kernel source prefixed with the given set of
 * compile-time #define values. Returns an RAII-wrapped program on success and
 * throws on build failure (after dumping the build log).
 */
static ocl::Program buildProgram(cl_context context, cl_device_id device,
		const std::string & defines, const std::string & kernelSource)
{
	const std::string fullSource = defines + kernelSource;
	const char * sources[] = { fullSource.c_str() };
	const size_t sourceLengths[] = { fullSource.length() };

	cl_int err;
	const cl_program program = clCreateProgramWithSource(context, 1, sources, sourceLengths, &err);
	if (err != CL_SUCCESS) throw std::runtime_error("Failed to create program: " + std::to_string(err));
	ocl::Program wrapped = ocl::Program::adapt({}, program);

	err = clBuildProgram(program, 1, &device, "-cl-fast-relaxed-math", nullptr, nullptr);
	if (err != CL_SUCCESS) {
		size_t log_size;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
		std::vector<char> log(log_size);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
		std::cout << "Build log:\n" << log.data() << "\n";
		throw std::runtime_error("Failed to build program: " + std::to_string(err));
	}

	return wrapped;
}

int main(int argc, char ** argv)
{
	try {
		const std::string kernel_file = "assets/matrix_multiply.cl";
		std::string csv_file;
		bool help = false;
		bool verbose = false;
		bool validate = false;
		int M = 512, N = 1536, P = 1024;
		int wg_width = 8, wg_height = 4;
		int square_wg_side = 16;
		int linear_wg_size = 32;
		int tile_square_side = 8;
		int tile_rect_m = 8, tile_rect_n = 64, tile_rect_p = 4;
		std::string kernel_name;

		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];
			if (arg == "-h") help = true;
			else if (arg == "-v") verbose = true;
			else if (arg == "-va") validate = true;
			else if (arg == "-csv" && i + 1 < argc) csv_file = argv[++i];
			else if (arg == "-kernel" && i + 1 < argc) kernel_name = argv[++i];
			else if (arg == "-m" && i + 1 < argc) M = std::stoi(argv[++i]);
			else if (arg == "-n" && i + 1 < argc) N = std::stoi(argv[++i]);
			else if (arg == "-p" && i + 1 < argc) P = std::stoi(argv[++i]);
			else if (arg == "-wg_square_side" && i + 1 < argc) square_wg_side = std::stoi(argv[++i]);
			else if (arg == "-wg_linear_size" && i + 1 < argc) linear_wg_size = std::stoi(argv[++i]);
			else if (arg == "-wg_rect_width" && i + 1 < argc) wg_width = std::stoi(argv[++i]);
			else if (arg == "-wg_rect_height" && i + 1 < argc) wg_height = std::stoi(argv[++i]);
			else if (arg == "-tile_square" && i + 1 < argc) tile_square_side = std::stoi(argv[++i]);
			else if (arg == "-tile_rect_m" && i + 1 < argc) tile_rect_m = std::stoi(argv[++i]);
			else if (arg == "-tile_rect_n" && i + 1 < argc) tile_rect_n = std::stoi(argv[++i]);
			else if (arg == "-tile_rect_p" && i + 1 < argc) tile_rect_p = std::stoi(argv[++i]);
		}

		VERBOSE = verbose;
		VALIDATE = validate;

		if (help) {
			printHelp();
			return 0;
		}

		std::cout << "\nSingle Precision General Matrix Multiplication (SGEMM) benchmarking test.\n";

		const std::string kernel_str = readTextFile(kernel_file);

		const ocl::Context context = ocl::Context::create();
		const cl_device_id device = context.findDevice();
		const ocl::CommandQueue commandQueue = context.createCommandQueue(device);

		size_t max_workgroup_size;
		CHECKCL(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,
				sizeof(max_workgroup_size), &max_workgroup_size, nullptr))
		if (static_cast<size_t>(wg_width * wg_height) > max_workgroup_size ||
				static_cast<size_t>(square_wg_side * square_wg_side) > max_workgroup_size ||
				static_cast<size_t>(linear_wg_size) > max_workgroup_size ||
				static_cast<size_t>(tile_rect_m * tile_rect_p) > max_workgroup_size) {
			throw std::runtime_error("Workgroup size exceeds device maximum");
		}

		Matrix A(M, N), B(N, P), C(M, P), CT(P, M);
		bool success = true;
		success &= assert_param("M must be a multiple of wg_height", M % wg_height == 0);
		success &= assert_param("P must be a multiple of wg_width", P % wg_width == 0);
		success &= assert_param("M must be a multiple of tile_square", M % tile_square_side == 0);
		success &= assert_param("N must be a multiple of tile_square", N % tile_square_side == 0);
		success &= assert_param("P must be a multiple of tile_square", P % tile_square_side == 0);
		success &= assert_param("N must be a multiple of wg_linear_size", N % linear_wg_size == 0);
		success &= assert_param("M must be a multiple of tile_rect_m", M % tile_rect_m == 0);
		success &= assert_param("N must be a multiple of tile_rect_n", N % tile_rect_n == 0);
		success &= assert_param("P must be a multiple of tile_rect_p", P % tile_rect_p == 0);
		success &= assert_param("tile_rect_n must be a multiple of lcm(tile_rect_m,tile_rect_p)*4",
				(tile_rect_n % (lcm(tile_rect_m, tile_rect_p) * 4) == 0));
		success &= assert_param("N must be a multiple of 4 for vectorized kernels", N % 4 == 0);
		if (!success) return 1;

		std::cout << "\nM: " << std::setw(6) << M << "     N: " << std::setw(6) << N
				<< "     P:" << std::setw(6) << P << "\n"
				<< "Left Matrix(MxN):  " << M << "x" << N
				<< "      Right Matrix(NxP): " << N << "x" << P << "\n";

		Timer mytime;
		std::cout << "==> Populating Matrices                       - Time: " << mytime.getElapsedMilliSecsF();
		mytime.Reset();

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> distA(-20.0f, 20.0f);
		std::uniform_real_distribution<float> distB(-10.0f, 10.0f);

		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j) A(i, j) = distA(gen);
			for (int j = 0; j < P; ++j) C(i, j) = 99999.0f;
		}
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < P; ++j) B(i, j) = distB(gen);
		}

		std::cout << "(" << mytime.getElapsedMilliSecsF() << "ms)\n"
				<< "==> Transposing Matrices                      - Time: " << mytime.getElapsedMilliSecsF();
		mytime.Reset();

		Matrix AT = transpose(A);
		Matrix BT = transpose(B);

		std::cout << "(" << mytime.getElapsedMilliSecsF() << "ms)\n"
				<< "==> Setting up buffers                        - Time: " << mytime.getElapsedMilliSecsF();
		mytime.Reset();

		const auto createBufferCopy = [&context] (const char * name, size_t bytes, void * hostPtr) {
			cl_int err;
			const cl_mem mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					bytes, hostPtr, &err);
			if (err != CL_SUCCESS) throw std::runtime_error(std::string("Failed to create buffer ") + name + ": " + std::to_string(err));
			return ocl::Mem::adapt({}, mem);
		};

		const ocl::Mem memA = createBufferCopy("A", A.size() * sizeof(float), A.data_ptr());
		const ocl::Mem memAT = createBufferCopy("AT", AT.size() * sizeof(float), AT.data_ptr());
		const ocl::Mem memB = createBufferCopy("B", B.size() * sizeof(float), B.data_ptr());
		const ocl::Mem memBT = createBufferCopy("BT", BT.size() * sizeof(float), BT.data_ptr());
		const ocl::Mem memC = context.createBuffer(CL_MEM_READ_WRITE, C.size() * sizeof(float));

		// Create float4 buffers for optimized kernel
		std::vector<float> A4_data(M * (N / 4) * 4);
		std::vector<float> BT4_data(P * (N / 4) * 4);
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j) {
				A4_data[i * N + j] = A(i, j);
			}
		}
		for (int i = 0; i < P; ++i) {
			for (int j = 0; j < N; ++j) {
				BT4_data[i * N + j] = BT(i, j);
			}
		}
		const ocl::Mem memA4 = createBufferCopy("A4", M * (N / 4) * 4 * sizeof(float), A4_data.data());
		const ocl::Mem memBT4 = createBufferCopy("BT4", P * (N / 4) * 4 * sizeof(float), BT4_data.data());

		std::cout << "(" << mytime.getElapsedMilliSecsF() << "ms)\n";

		Matrix D(M, P), DT(P, M);
		if (VALIDATE) {
			std::cout << "==> Preparing ground truth: CPU-side multiply  - Time: " << mytime.getElapsedMilliSecsF();
			mytime.Reset();
			D = A * B;
			DT = transpose(D);
			std::cout << "(" << mytime.getElapsedMilliSecsF() << "ms)\n";
		}

		KernelParams k;
		k.queue = commandQueue;
		k.mytime = mytime;
		k.globalSize[0] = M;
		k.globalSize[1] = P;
		k.localSize[0] = 0;
		k.localSize[1] = 0;

		std::array<std::string, 14> kernel_names = {
			"matmul_naive", "matmul_transposedA", "matmul_transposedB", "matmul_transposedC",
			"matmul_transposedAC", "matmul_transposedBC", "matmul_linearwg_AT", "matmul_linearwg_BT",
			"matmul_linearwg_vec4", "matmul_linearwg_vec4_local", "matmul_tile_square",
			"matmul_tile_rect", "matmul_tile_rect_vec4", "matmul_optimized"
		};
		std::array<bool, 14> kernel_bools;
		std::fill(kernel_bools.begin(), kernel_bools.end(), kernel_name.empty());
		if (!kernel_name.empty()) {
			auto it = std::find(kernel_names.begin(), kernel_names.end(), kernel_name);
			if (it != kernel_names.end()) kernel_bools[it - kernel_names.begin()] = true;
		}

		const std::string defines = "#define M " + std::to_string(M) + "\n"
				"#define N " + std::to_string(N) + "\n"
				"#define P " + std::to_string(P) + "\n"
				"#define WG_RECT_WIDTH " + std::to_string(wg_width) + "\n"
				"#define WG_RECT_HEIGHT " + std::to_string(wg_height) + "\n"
				"#define WG_SQUARE_SIDE " + std::to_string(square_wg_side) + "\n"
				"#define WG_LINEAR_SIZE " + std::to_string(linear_wg_size) + "\n"
				"#define TILE_SQUARE " + std::to_string(tile_square_side) + "\n"
				"#define TILE_RECT_M " + std::to_string(tile_rect_m) + "\n"
				"#define TILE_RECT_N " + std::to_string(tile_rect_n) + "\n"
				"#define TILE_RECT_P " + std::to_string(tile_rect_p) + "\n";

		std::cout << "==> Creating OpenCL program                   - Time: " << mytime.getElapsedMilliSecsF() << "\n";
		const ocl::Program program = buildProgram(context, device, defines, kernel_str);

		k.program = program;

		k.localSize[0] = wg_width;
		k.localSize[1] = wg_height;
		if (kernel_bools[0]) try_catch_execute_kernel(k, "matmul_naive", memA, memB, memC, C, D, "", M, N, P);
		if (kernel_bools[1]) try_catch_execute_kernel(k, "matmul_transposedA", memAT, memB, memC, C, D, "", M, N, P);
		if (kernel_bools[2]) try_catch_execute_kernel(k, "matmul_transposedB", memA, memBT, memC, C, D, "", M, N, P);
		if (kernel_bools[3]) try_catch_execute_kernel(k, "matmul_transposedC", memA, memB, memC, CT, DT, "", M, N, P);
		if (kernel_bools[4]) try_catch_execute_kernel(k, "matmul_transposedAC", memAT, memB, memC, CT, DT, "", M, N, P);
		if (kernel_bools[5]) try_catch_execute_kernel(k, "matmul_transposedBC", memA, memBT, memC, CT, DT, "", M, N, P);

		k.localSize[0] = 1;
		k.localSize[1] = linear_wg_size;
		if (kernel_bools[6]) try_catch_execute_kernel(k, "matmul_linearwg_AT", memAT, memB, memC, C, D, "", M, N, P);
		if (kernel_bools[7]) try_catch_execute_kernel(k, "matmul_linearwg_BT", memA, memBT, memC, C, D, "", M, N, P);
		if (kernel_bools[8]) try_catch_execute_kernel(k, "matmul_linearwg_vec4", memA, memBT, memC, C, D, "", M, N, P);
		if (kernel_bools[9]) try_catch_execute_kernel(k, "matmul_linearwg_vec4_local", memA, memBT, memC, C, D, "", M, N, P);

		k.localSize[0] = square_wg_side;
		k.localSize[1] = square_wg_side;
		if (kernel_bools[10]) try_catch_execute_kernel(k, "matmul_tile_square", memA, memBT, memC, C, D, "", M, N, P);

		k.localSize[0] = tile_rect_m;
		k.localSize[1] = tile_rect_p;
		if (kernel_bools[11]) try_catch_execute_kernel(k, "matmul_tile_rect", memA, memBT, memC, C, D, "", M, N, P);

		if (kernel_name.empty()) {
			int pmin = 4, pmax = 32, mmin = 2, mmax = 32, nmax = 128;
			for (int p = pmin; p <= pmax; p <<= 1) {
				for (int m = mmin; m <= mmax; m <<= 1) {
					for (int n = lcm(p * 4, m * 4); n <= nmax; n <<= 1) {
						if (static_cast<size_t>(m * p) > max_workgroup_size) continue;
						std::stringstream ss;
						ss << m << "x" << n << "x" << p;
						try {
							const std::string def = "#define M " + std::to_string(M) + "\n"
									"#define N " + std::to_string(N) + "\n"
									"#define P " + std::to_string(P) + "\n"
									"#define WG_RECT_WIDTH " + std::to_string(wg_width) + "\n"
									"#define WG_RECT_HEIGHT " + std::to_string(wg_height) + "\n"
									"#define WG_SQUARE_SIDE " + std::to_string(square_wg_side) + "\n"
									"#define WG_LINEAR_SIZE " + std::to_string(linear_wg_size) + "\n"
									"#define TILE_SQUARE " + std::to_string(tile_square_side) + "\n"
									"#define TILE_RECT_M " + std::to_string(m) + "\n"
									"#define TILE_RECT_N " + std::to_string(n) + "\n"
									"#define TILE_RECT_P " + std::to_string(p) + "\n";

							if (VERBOSE) {
								std::cout << "\n==> Creating OpenCL program " << ss.str()
										<< " - Time: " << mytime.getElapsedMilliSecsF();
							}

							const ocl::Program temp_program = buildProgram(context, device, def, kernel_str);

							k.program = temp_program;
							k.localSize[0] = m;
							k.localSize[1] = p;
							if (kernel_bools[12]) try_catch_execute_kernel(k, "matmul_tile_rect_vec4", memA, memBT, memC, C, D, ss.str(), M, N, P);
							if (kernel_bools[13]) try_catch_execute_kernel(k, "matmul_optimized", memA4, memBT4, memC, C, D, ss.str(), M, N, P, true);
							k.program = program;
						} catch (const std::exception & e) {
							std::cout << "+++ Kernel matmul_tile_rect_vec4_" << ss.str()
									<< " failed to execute: " << e.what() << "!\n";
						}
					}
				}
			}
		}

		print_results(M, N, P, csv_file);
	} catch (const std::exception & e) {
		std::cout << "Error encountered: " << e.what() << "\n";
	} catch (...) {
		std::cout << "Execution encountered an unknown error\n";
	}

	std::cout << "\n";
	return 0;
}
