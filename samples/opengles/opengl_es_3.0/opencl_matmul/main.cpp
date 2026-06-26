/*******************************************************************************
 *
 * Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
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
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 *******************************************************************************/
/******************************************************************************/
/**
 *
 * @file OpenCLMatrixMul.cpp
 *
 * This file implements OpenCL Matrix Mul
 *
 * @note        None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           -------- -----------------------------------------------
 * 1.0   Alok G         07/07/2025        Initial release.
 * </pre>
 *
 *******************************************************************************/
/******************************* Source Files ********************************/
#define CL_TARGET_OPENCL_VERSION 300

#include <CL/cl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <string>

// Scale based on GPU used
// For ARM A78, scale is approximately 1.0
// For NVIDIA RTX A2000, scale is 0.0325

// Matrix dimensions
#define M 512
#define N 1536
#define P 1024

// Workgroup sizes
#define WG_RECT_WIDTH 16
#define WG_RECT_HEIGHT 16
#define WG_LINEAR_SIZE 64
#define WG_SQUARE_SIDE 16
#define TILE_SQUARE WG_SQUARE_SIDE
#define TILE_RECT_N 64 // Must be multiple of TILE_RECT_M and TILE_RECT_P
// Tile configurations for matmul_tile_rect_vec4
typedef struct {
    int m, n, p;
} TileConfig;

TileConfig tile_configs[] = {{2, 16, 4}, {2, 32, 4},  {2, 64, 4},  {4, 16, 4},  {4, 32, 4},  {4, 64, 4},  {8, 32, 4},
                             {8, 64, 4}, {16, 64, 4}, {2, 32, 8},  {2, 64, 8},  {4, 32, 8},  {4, 64, 8},  {8, 32, 8},
                             {8, 64, 8}, {16, 64, 8}, {2, 64, 16}, {4, 64, 16}, {8, 64, 16}, {16, 64, 16}};
const int num_tile_configs = sizeof(tile_configs) / sizeof(tile_configs[0]);

// Macro to check OpenCL errors
#define CHECK_CL_ERROR(err, msg)                                                                                       \
    do {                                                                                                               \
        if (err != CL_SUCCESS) {                                                                                       \
            fprintf(stderr, "OpenCL Error at %s: %d\n", msg, err);                                                     \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    } while (0)

// Read kernel source from file
char *read_kernel_source(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open kernel file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char *source = (char *)malloc(size + 1);
    if (fread(source, 1, size, file) != size) {
        fprintf(stderr, "Failed to read kernel file: %s\n", filename);
        free(source);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    source[size] = '\0';
    fclose(file);
    return source;
}

// CPU reference implementation for verification
void matmul_cpu(float *A, float *B, float *C, int m, int n, int p) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            float sum = 0.0f;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * p + j];
            }
            C[i * p + j] = sum;
        }
    }
}

// Verify GPU results against CPU results
int verify_results(float *gpu_C, float *cpu_C, int m, int p, int transposed, const char *kernel_name) {
    float epsilon = 5e-3;
    int failed = 0;
    float max_diff = 0.0f;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            float gpu_val, cpu_val;
            if (transposed) {
                gpu_val = gpu_C[j * m + i];
                cpu_val = cpu_C[i * p + j];
            } else {
                gpu_val = gpu_C[i * p + j];
                cpu_val = cpu_C[i * p + j];
            }
            float diff = fabs(gpu_val - cpu_val);
            if (diff > epsilon) {
                if (i < 2 && j < 2) { // Print first few errors for debugging
                    printf("Verification failed for %s at (%d,%d): GPU=%f, CPU=%f, "
                           "Diff=%e\n",
                           kernel_name, i, j, gpu_val, cpu_val, diff);
                }
                failed = 1;
                if (diff > max_diff)
                    max_diff = diff;
            }
        }
    }
    if (failed) {
        printf("Max difference for %s: %e\n", kernel_name, max_diff);
    }
    return !failed;
}

// Convert float array to float4 array
void convert_to_float4(float *src, cl_float4 *dst, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols / 4; j++) {
            dst[i * (cols / 4) + j] = (cl_float4){src[i * cols + j * 4], src[i * cols + j * 4 + 1],
                                                  src[i * cols + j * 4 + 2], src[i * cols + j * 4 + 3]};
        }
    }
}

float detect_gpu_and_get_scale() {
    cl_platform_id platform;
    cl_device_id device;
    char vendor[128];
    char device_name[128];

    float scale = 1.0f; // default

    if (clGetPlatformIDs(1, &platform, NULL) == CL_SUCCESS &&
        clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL) == CL_SUCCESS &&
        clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL) == CL_SUCCESS &&
        clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL) == CL_SUCCESS) {
        printf("Detected GPU Vendor: %s\n", vendor);
        printf("Detected GPU Device: %s\n", device_name);
        if (strstr(vendor, "NVIDIA")) {
            if (strstr(device_name, "RTX A2000")) {
                scale = 0.0125f; // Specific scale for RTX A2000
            }
        } else if (strstr(vendor, "AMD") || strstr(vendor, "Advanced Micro Devices")) {
            if (strstr(device_name, "gfx1152")) { // Ryzen 5 AI Pro
                scale = 0.2f;
            } else {
                scale = 0.1f;
            }
        } else if (strstr(vendor, "ARM")) {
            if (strstr(device_name, "Mali-G78AE")) {
                scale = 1.0f;
            }
        } else {
            scale = 0.5f;
        }
    }

    return scale;
}

int main(int argc, char *argv[]) {
    float scale;

    if (argc == 2) {
        scale = std::stof(argv[1]);
    } else {
        scale = detect_gpu_and_get_scale();
    }
    // Initialize random seed
    srand(time(NULL));

    printf(" scale: %.2f\n", scale);

    // Allocate host matrices (row-major)
    float *A = (float *)malloc(M * N * sizeof(float));
    float *B = (float *)malloc(N * P * sizeof(float));
    float *C = (float *)malloc(M * P * sizeof(float));
    float *CT = (float *)malloc(P * M * sizeof(float));
    float *C_cpu = (float *)malloc(M * P * sizeof(float));
    float *AT = (float *)malloc(N * M * sizeof(float));
    float *BT = (float *)malloc(P * N * sizeof(float));
    cl_float4 *A4 = (cl_float4 *)malloc(M * (N / 4) * sizeof(cl_float4));
    cl_float4 *BT4 = (cl_float4 *)malloc(P * (N / 4) * sizeof(cl_float4));

    // Initialize matrices A and B with scaled random values
    clock_t start = clock();
    for (int i = 0; i < M * N; i++)
        A[i] = 0.01f * ((float)rand() / RAND_MAX);
    for (int i = 0; i < N * P; i++)
        B[i] = 0.01f * ((float)rand() / RAND_MAX);
    printf("==> Populating Matrices                       - Time: %.6f ms\n",
           (double)(clock() - start) / CLOCKS_PER_SEC * 1000);

    // Compute transposes
    start = clock();
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            AT[j * M + i] = A[i * N + j];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < P; j++)
            BT[j * N + i] = B[i * P + j];
    printf("==> Transposing Matrices                      - Time: %.6f ms\n",
           (double)(clock() - start) / CLOCKS_PER_SEC * 1000);

    // Convert to float4
    convert_to_float4(A, A4, M, N);
    convert_to_float4(BT, BT4, P, N);

    // Compute CPU reference result
    start = clock();
    matmul_cpu(A, B, C_cpu, M, N, P);
    printf("==> Preparing ground truth: CPU-side multiply - Time: %.6f ms\n",
           (double)(clock() - start) / CLOCKS_PER_SEC * 1000);

    // OpenCL setup
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_mem d_A, d_B, d_C, d_A4, d_BT4;

    // Get platform and device
    start = clock();
    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK_CL_ERROR(err, "clGetPlatformIDs");
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    CHECK_CL_ERROR(err, "clGetDeviceIDs");

    // Create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_CL_ERROR(err, "clCreateContext");
    printf("==> Creating OpenCL context                  - Time: %.6f ms\n",
           (double)(clock() - start) / CLOCKS_PER_SEC * 1000);

    // Create command queue with properties
    start = clock();
    cl_command_queue_properties props[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
    queue = clCreateCommandQueueWithProperties(context, device, props, &err);
    CHECK_CL_ERROR(err, "clCreateCommandQueueWithProperties");

    // Read and build program
    char *kernel_source = read_kernel_source("cl_kernels/matmul.cl");
    program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, NULL, &err);
    CHECK_CL_ERROR(err, "clCreateProgramWithSource");
    printf("==> Creating OpenCL program                  - Time: %.6f ms\n",
           (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
    free(kernel_source);

    // Create buffers
    start = clock();
    d_A = clCreateBuffer(context, CL_MEM_READ_ONLY, M * N * sizeof(float), NULL, &err);
    CHECK_CL_ERROR(err, "clCreateBuffer A");
    d_B = clCreateBuffer(context, CL_MEM_READ_ONLY, N * P * sizeof(float), NULL, &err);
    CHECK_CL_ERROR(err, "clCreateBuffer B");
    d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, M * P * sizeof(float), NULL, &err);
    CHECK_CL_ERROR(err, "clCreateBuffer C");
    d_A4 = clCreateBuffer(context, CL_MEM_READ_ONLY, M * (N / 4) * sizeof(cl_float4), NULL, &err);
    CHECK_CL_ERROR(err, "clCreateBuffer A4");
    d_BT4 = clCreateBuffer(context, CL_MEM_READ_ONLY, P * (N / 4) * sizeof(cl_float4), NULL, &err);
    CHECK_CL_ERROR(err, "clCreateBuffer BT4");
    printf("==> Setting up buffers                       - Time: %.6f ms\n",
           (double)(clock() - start) / CLOCKS_PER_SEC * 1000);

    // List of kernels to test
    typedef struct {
        const char *name;
        int tile_m, tile_n, tile_p;
        const char *category;
    } KernelInfo;
    KernelInfo kernel_info[] = {{"matmul_naive", 0, 0, 0, "Naive"},
                                {"matmul_transposedA", 0, 0, 0, "Transposed"},
                                {"matmul_transposedB", 0, 0, 0, "Transposed"},
                                {"matmul_transposedC", 0, 0, 0, "Transposed"},
                                {"matmul_transposedAC", 0, 0, 0, "Transposed"},
                                {"matmul_transposedBC", 0, 0, 0, "Transposed"},
                                {"matmul_linearwg_AT", 0, 0, 0, "Linear Workgroup"},
                                {"matmul_linearwg_BT", 0, 0, 0, "Linear Workgroup"},
                                {"matmul_linearwg_vec4", 0, 0, 0, "Vectorized"},
                                {"matmul_linearwg_vec4_local", 0, 0, 0, "Vectorized"},
                                {"matmul_tile_square", 0, 0, 0, "Tiled"},
                                {"matmul_tile_rect", 0, 0, 0, "Tiled"}};
    const int num_base_kernels = sizeof(kernel_info) / sizeof(kernel_info[0]);

    // Dynamically add tile_rect_vec4 variants
    KernelInfo *kernels = (KernelInfo *)malloc((num_base_kernels + num_tile_configs) * sizeof(KernelInfo));
    memcpy(kernels, kernel_info, num_base_kernels * sizeof(KernelInfo));
    for (int i = 0; i < num_tile_configs; i++) {
        char *name = (char *)malloc(50);
        snprintf(name, 50, "matmul_tile_rect_vec4 %dx%dx%d", tile_configs[i].m, tile_configs[i].n, tile_configs[i].p);
        kernels[num_base_kernels + i] =
            (KernelInfo){name, tile_configs[i].m, tile_configs[i].n, tile_configs[i].p, "Vectorized Tiled"};
    }
    const int num_kernels = num_base_kernels + num_tile_configs;

    // Build programs for each tile configuration
    cl_program *programs = (cl_program *)malloc(num_kernels * sizeof(cl_program));
    for (int k = 0; k < num_kernels; k++) {
        char build_options[256];
        if (strstr(kernels[k].name, "matmul_tile_rect_vec4")) {
            snprintf(build_options, sizeof(build_options),
                     "-D M=%d -D N=%d -D P=%d "
                     "-D WG_RECT_WIDTH=%d -D WG_RECT_HEIGHT=%d "
                     "-D WG_LINEAR_SIZE=%d -D WG_SQUARE_SIDE=%d "
                     "-D TILE_SQUARE=%d -D TILE_RECT_M=%d "
                     "-D TILE_RECT_N=%d -D TILE_RECT_P=%d",
                     M, N, P, WG_RECT_WIDTH, WG_RECT_HEIGHT, WG_LINEAR_SIZE, WG_SQUARE_SIDE, TILE_SQUARE,
                     kernels[k].tile_m, kernels[k].tile_n, kernels[k].tile_p);
        } else {
            snprintf(build_options, sizeof(build_options),
                     "-D M=%d -D N=%d -D P=%d "
                     "-D WG_RECT_WIDTH=%d -D WG_RECT_HEIGHT=%d "
                     "-D WG_LINEAR_SIZE=%d -D WG_SQUARE_SIDE=%d "
                     "-D TILE_SQUARE=%d -D TILE_RECT_M=%d "
                     "-D TILE_RECT_N=%d -D TILE_RECT_P=%d",
                     M, N, P, WG_RECT_WIDTH, WG_RECT_HEIGHT, WG_LINEAR_SIZE, WG_SQUARE_SIDE, TILE_SQUARE, 16,
                     TILE_RECT_N, 16);
        }
        char *kernel_source = read_kernel_source("cl_kernels/matmul.cl");
        programs[k] = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, NULL, &err);
        CHECK_CL_ERROR(err, "clCreateProgramWithSource");
        free(kernel_source);
        err = clBuildProgram(programs[k], 1, &device, build_options, NULL, NULL);
        if (err != CL_SUCCESS) {
            size_t log_size;
            clGetProgramBuildInfo(programs[k], device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
            char *log = (char *)malloc(log_size);
            clGetProgramBuildInfo(programs[k], device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
            fprintf(stderr, "Build error for %s: %s\n", kernels[k].name, log);
            free(log);
            exit(EXIT_FAILURE);
        }
    }

    // Run each kernel and measure performance
    printf("\nRunning SGEMM Benchmark (M=%d, N=%d, P=%d)\n", M, N, P);
    printf("Left Matrix: %dx%d, Right Matrix: %dx%d\n", M, N, N, P);
    printf("\nPerformance Rankings by Optimization Category\n");
    printf("========================================================================"
           "========\n");
    printf("%-20s | %-25s | %-10s | %-10s | %-10s | %-10s\n", "Category", "Kernel", "Time (ms)", "GFLOPS", "Speedup",
           "Status");
    printf("========================================================================"
           "========\n");

    typedef struct {
        const char *name;
        const char *category;
        double time;
        double gflops;
        double speedup;
        int verified;
    } Result;
    Result *results = (Result *)malloc(num_kernels * sizeof(Result));
    int result_count = 0;
    double naive_time = 0.0;

    for (int k = 0; k < num_kernels; k++) {
        cl_kernel kernel;
        if (strstr(kernels[k].name, "matmul_tile_rect_vec4")) {
            char kernel_name[50];
            snprintf(kernel_name, 50, "matmul_tile_rect_vec4");
            kernel = clCreateKernel(programs[k], kernel_name, &err);
        } else {
            kernel = clCreateKernel(programs[k], kernels[k].name, &err);
        }
        CHECK_CL_ERROR(err, "clCreateKernel");

        // Set input/output buffers based on kernel
        int transposed_c = 0;
        if (strcmp(kernels[k].name, "matmul_naive") == 0 || strcmp(kernels[k].name, "matmul_linearwg_BT") == 0 ||
            strcmp(kernels[k].name, "matmul_tile_square") == 0 || strcmp(kernels[k].name, "matmul_tile_rect") == 0) {
            err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, M * N * sizeof(float), A, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer A");
            err = clEnqueueWriteBuffer(queue, d_B, CL_TRUE, 0, N * P * sizeof(float), B, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer B");
            err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
            err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
            err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
            CHECK_CL_ERROR(err, "clSetKernelArg");
        } else if (strcmp(kernels[k].name, "matmul_transposedA") == 0 ||
                   strcmp(kernels[k].name, "matmul_linearwg_AT") == 0) {
            err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, N * M * sizeof(float), AT, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer AT");
            err = clEnqueueWriteBuffer(queue, d_B, CL_TRUE, 0, N * P * sizeof(float), B, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer B");
            err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
            err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
            err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
            CHECK_CL_ERROR(err, "clSetKernelArg");
        } else if (strcmp(kernels[k].name, "matmul_transposedB") == 0) {
            err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, M * N * sizeof(float), A, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer A");
            err = clEnqueueWriteBuffer(queue, d_B, CL_TRUE, 0, P * N * sizeof(float), BT, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer BT");
            err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
            err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
            err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
            CHECK_CL_ERROR(err, "clSetKernelArg");
        } else if (strcmp(kernels[k].name, "matmul_transposedC") == 0) {
            err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, M * N * sizeof(float), A, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer A");
            err = clEnqueueWriteBuffer(queue, d_B, CL_TRUE, 0, N * P * sizeof(float), B, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer B");
            err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
            err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
            err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
            CHECK_CL_ERROR(err, "clSetKernelArg");
            transposed_c = 1;
        } else if (strcmp(kernels[k].name, "matmul_transposedAC") == 0) {
            err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, N * M * sizeof(float), AT, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer AT");
            err = clEnqueueWriteBuffer(queue, d_B, CL_TRUE, 0, N * P * sizeof(float), B, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer B");
            err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
            err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
            err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
            CHECK_CL_ERROR(err, "clSetKernelArg");
            transposed_c = 1;
        } else if (strcmp(kernels[k].name, "matmul_transposedBC") == 0) {
            err = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, M * N * sizeof(float), A, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer A");
            err = clEnqueueWriteBuffer(queue, d_B, CL_TRUE, 0, P * N * sizeof(float), BT, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer BT");
            err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
            err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
            err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
            CHECK_CL_ERROR(err, "clSetKernelArg");
            transposed_c = 1;
        } else if (strstr(kernels[k].name, "matmul_linearwg_vec4") ||
                   strstr(kernels[k].name, "matmul_tile_rect_vec4")) {
            err = clEnqueueWriteBuffer(queue, d_A4, CL_TRUE, 0, M * (N / 4) * sizeof(cl_float4), A4, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer A4");
            err = clEnqueueWriteBuffer(queue, d_BT4, CL_TRUE, 0, P * (N / 4) * sizeof(cl_float4), BT4, 0, NULL, NULL);
            CHECK_CL_ERROR(err, "clEnqueueWriteBuffer BT4");
            err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A4);
            err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_BT4);
            err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);
            CHECK_CL_ERROR(err, "clSetKernelArg");
        }

        // Set global and local workgroup sizes
        size_t global_size[2] = {M, P};
        size_t local_size[2];
        if (strstr(kernels[k].name, "linearwg")) {
            local_size[0] = 1;
            local_size[1] = WG_LINEAR_SIZE;
        } else if (strcmp(kernels[k].name, "matmul_tile_square") == 0) {
            local_size[0] = WG_SQUARE_SIDE;
            local_size[1] = WG_SQUARE_SIDE;
        } else if (strcmp(kernels[k].name, "matmul_tile_rect") == 0) {
            local_size[0] = 16;
            local_size[1] = 16;
        } else if (strstr(kernels[k].name, "matmul_tile_rect_vec4")) {
            local_size[0] = kernels[k].tile_m;
            local_size[1] = kernels[k].tile_p;
        } else {
            local_size[0] = WG_RECT_WIDTH;
            local_size[1] = WG_RECT_HEIGHT;
        }

        // Warm-up run
        err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, local_size, 0, NULL, NULL);
        CHECK_CL_ERROR(err, "clEnqueueNDRangeKernel warm-up");
        clFinish(queue);

        // Timed run
        cl_event event;
        const int num_runs = 10;
        double total_time = 0.0;
        for (int run = 0; run < num_runs; run++) {
            err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, local_size, 0, NULL, &event);
            CHECK_CL_ERROR(err, "clEnqueueNDRangeKernel");
            clWaitForEvents(1, &event);

            cl_ulong start, end;
            clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
            clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
            total_time += (end - start) / 1e6; // Convert to milliseconds
            clReleaseEvent(event);
        }
        double avg_time = total_time / num_runs;
        double gflops = (2.0 * M * N * P) / (avg_time * 1e-3) / 1e9; // GFLOPS

        // Read back results
        err = clEnqueueReadBuffer(queue, d_C, CL_TRUE, 0, (transposed_c ? P * M : M * P) * sizeof(float),
                                  (transposed_c ? CT : C), 0, NULL, NULL);
        CHECK_CL_ERROR(err, "clEnqueueReadBuffer C/CT");

        // Verify results
        int verified = verify_results(transposed_c ? CT : C, C_cpu, M, P, transposed_c, kernels[k].name);

        // Store naive time for speedup calculation
        if (strcmp(kernels[k].name, "matmul_naive") == 0) {
            naive_time = avg_time;
        }

        // Calculate speedup
        double speedup = naive_time / avg_time;

        // Determine performance tier
        const char *tier;
        if (avg_time < 200.0 * scale)
            tier = "Excellent";
        else if (avg_time < 300.0 * scale)
            tier = "Good";
        else if (avg_time < 400.0 * scale)
            tier = "Fair";
        else
            tier = "Poor";

        // Store results
        results[result_count++] = (Result){kernels[k].name, kernels[k].category, avg_time, gflops, speedup, verified};

        // Print results
        printf("%-20s | %-25s | %-10.2f | %-10.2f | %-10.2f | %-10s\n", kernels[k].category, kernels[k].name, avg_time,
               gflops, speedup, verified ? tier : "Failed");

        clReleaseKernel(kernel);
    }

    // Category-wise ranking and summary
    printf("\nCategory-Wise Performance Summary\n");
    printf("========================================================================"
           "========\n");
    const char *categories[] = {"Naive", "Transposed", "Linear Workgroup", "Vectorized", "Tiled", "Vectorized Tiled"};
    int num_categories = 6;
    for (int c = 0; c < num_categories; c++) {
        printf("\nCategory: %s\n", categories[c]);
        printf("----------------------------------------\n");
        printf("%-25s | %-10s | %-10s | %-10s\n", "Kernel", "Time (ms)", "GFLOPS", "Speedup");
        printf("----------------------------------------\n");

        // Collect results for this category
        Result *cat_results = (Result *)malloc(num_kernels * sizeof(Result));
        int cat_count = 0;
        for (int i = 0; i < result_count; i++) {
            if (strcmp(results[i].category, categories[c]) == 0) {
                cat_results[cat_count++] = results[i];
            }
        }

        // Sort by GFLOPS (descending)
        for (int i = 0; i < cat_count - 1; i++) {
            for (int j = i + 1; j < cat_count; j++) {
                if (cat_results[j].gflops > cat_results[i].gflops) {
                    Result temp = cat_results[i];
                    cat_results[i] = cat_results[j];
                    cat_results[j] = temp;
                }
            }
        }

        // Calculate average GFLOPS
        double avg_gflops = 0.0;
        for (int i = 0; i < cat_count; i++) {
            avg_gflops += cat_results[i].gflops;
            printf("%-25s | %-10.2f | %-10.2f | %-10.2f\n", cat_results[i].name, cat_results[i].time,
                   cat_results[i].gflops, cat_results[i].speedup);
        }
        if (cat_count > 0) {
            avg_gflops /= cat_count;
            printf("Average GFLOPS for %s: %.2f\n", categories[c], avg_gflops);
        } else {
            printf("No kernels in this category.\n");
        }
        free(cat_results);
    }

    // Verification summary
    printf("\nVerification Summary\n");
    printf("========================================================================"
           "========\n");
    int failed_count = 0;
    for (int i = 0; i < result_count; i++) {
        if (!results[i].verified) {
            failed_count++;
            printf("Kernel %-25s failed verification. Check debug output for details.\n", results[i].name);
        }
    }
    printf("Total kernels tested: %d, Verified: %d, Failed: %d\n", result_count, result_count - failed_count,
           failed_count);

    // Cleanup
    for (int k = 0; k < num_kernels; k++) {
        clReleaseProgram(programs[k]);
        if (strstr(kernels[k].name, "matmul_tile_rect_vec4")) {
            free((char *)kernels[k].name);
        }
    }
    free(kernels);
    free(programs);
    free(results);
    clReleaseMemObject(d_A);
    clReleaseMemObject(d_B);
    clReleaseMemObject(d_C);
    clReleaseMemObject(d_A4);
    clReleaseMemObject(d_BT4);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(A);
    free(B);
    free(C);
    free(CT);
    free(C_cpu);
    free(AT);
    free(BT);
    free(A4);
    free(BT4);

    return 0;
}
