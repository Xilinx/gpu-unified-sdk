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


#include <cstdlib>

#include <OpenCL/CLUtils.hpp>


/**
 * \brief Initialize the input matrices with random values.
 * \param[in] matrixOrder The order of the matrices (number of rows and columns). Matrices have to be symmetric.
 * \param[in] matrixA First input matrix.
 * \param[in] matrixB Second input matrix.
 * \param[in] matrixC Third input matrix.
 */
static void sgemmInitialize(int matrixOrder, float * matrixA, float * matrixB, float * matrixC)
{
	for (int i = 0; i < matrixOrder; i++) {
		for (int j = 0; j < matrixOrder; j++) {
			const int index = i * matrixOrder + j;

			/* Keep the values in the range [-1, 1]. */
			matrixA[index] = rand() / static_cast<float>(RAND_MAX) * 2 - 1;
			matrixB[index] = rand() / static_cast<float>(RAND_MAX) * 2 - 1;
			matrixC[index] = rand() / static_cast<float>(RAND_MAX) * 2 - 1;
		}
	}
}

/**
 * \brief Simple SGEMM OpenCL sample.
 * \details A sample which calculates the following SGEMM equation:
 *          matrixC = alpha * (matrixA * matrixB) + beta * matrixC.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/sgemm.cl");
	const ocl::Kernel kernel = program.createKernel("sgemm");

	const cl_uint matrixOrder = 2048;
	const cl_float alpha = 1.0f;
	const cl_float beta = 0.1f;

	const size_t matrixSize = static_cast<size_t>(matrixOrder) * matrixOrder;
	const int64_t bufferSize = matrixSize * sizeof(float);

	const ocl::Mem matrixAMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem matrixBMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem matrixCMem = context.createBuffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);

	{
		const ocl::EnqueueBufferRet<cl_float> matrixA = commandQueue.enqueueMapBuffer<cl_float>(
				matrixAMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		const ocl::EnqueueBufferRet<cl_float> matrixB = commandQueue.enqueueMapBuffer<cl_float>(
				matrixBMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		const ocl::EnqueueBufferRet<cl_float> matrixC = commandQueue.enqueueMapBuffer<cl_float>(
				matrixCMem, true, CL_MAP_WRITE, 0, bufferSize, false);

		sgemmInitialize(matrixOrder, matrixA.memory.data(), matrixB.memory.data(),
				matrixC.memory.data());
	}

	kernel.setArg<cl_mem>(0, matrixAMem);
	kernel.setArg<cl_mem>(1, matrixBMem);
	kernel.setArg<cl_mem>(2, matrixCMem);
	kernel.setArg<cl_uint>(3, matrixOrder);
	kernel.setArg<cl_float>(4, alpha);
	kernel.setArg<cl_float>(5, beta);

	/*
	 * Each kernel outputs one element in matrixC, therefore the total number of work items must be
	 * the number of elements (matrixOrder * matrixOrder).
	 */
	const size_t globalWorkSize[2] = { matrixOrder, matrixOrder };

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorkSize, nullptr, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	/* Map the output to a host side pointer (the result could be inspected here). */
	{
		const ocl::EnqueueBufferRet<cl_float> matrixC = commandQueue.enqueueMapBuffer<cl_float>(
				matrixCMem, true, CL_MAP_READ, 0, bufferSize, false);
		(void)matrixC;
	}

	return 0;
}
