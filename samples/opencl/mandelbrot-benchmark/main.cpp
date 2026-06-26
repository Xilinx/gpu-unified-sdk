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


#include <chrono>
#include <iostream>
#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


/**
 * \brief Mandelbrot set generation with performance benchmarking using OpenCL.
 * \details Computes the Mandelbrot fractal for a 1024x1024 grid, perturbs it with an input image,
 *          and measures performance on the GPU.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const int width = 1024;
	const int height = 1024;

	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/mandelbrot_bench.cl");
	const ocl::Kernel kernel = program.createKernel("compute_mandelbrot");

	int inputWidth, inputHeight;
	unsigned char * inputImageData = nullptr;
	loadFromBitmap("assets/input.bmp", &inputWidth, &inputHeight, &inputImageData);
	if (inputWidth != width || inputHeight != height) {
		delete[] inputImageData;
		std::cerr << "Input image must match 1024x1024." << std::endl;
		return 1;
	}

	const int64_t bufferSize = width * height * sizeof(cl_uchar);

	const ocl::Mem perturbationMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem unusedMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem outputMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	(void)unusedMem;

	{
		const ocl::EnqueueBufferRet<cl_uchar> perturbation = commandQueue.enqueueMapBuffer<cl_uchar>(
				perturbationMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		RGBToLuminance(inputImageData, perturbation.memory.data(), width, height);
	}
	delete[] inputImageData;

	kernel.setArg<cl_mem>(0, perturbationMem);
	kernel.setArg<cl_int>(1, width);
	kernel.setArg<cl_int>(2, height);
	kernel.setArg<cl_mem>(3, outputMem);

	const size_t globalWorkSize[2] = {
		static_cast<size_t>(width / 16),
		static_cast<size_t>(height),
	};
	const size_t localWorkSize[2] = { 16, 1 };

	const auto start = std::chrono::high_resolution_clock::now();

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorkSize, localWorkSize, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	const auto end = std::chrono::high_resolution_clock::now();
	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	const std::chrono::duration<double, std::milli> duration = end - start;
	const int totalIterations = width * height * 100;
	const double iterationsPerSecond = totalIterations / (duration.count() / 1000.0);
	std::cout << "Total Time: " << duration.count() << " ms" << std::endl;
	std::cout << "Iterations per Second: " << iterationsPerSecond << std::endl;

	{
		const ocl::EnqueueBufferRet<cl_uchar> output = commandQueue.enqueueMapBuffer<cl_uchar>(
				outputMem, true, CL_MAP_READ, 0, bufferSize, false);

		std::vector<unsigned char> rgbOut(width * height * 3);
		luminanceToRGB(output.memory.data(), rgbOut.data(), width, height);
		saveToBitmap("output.bmp", width, height, rgbOut.data());
	}

	return 0;
}
