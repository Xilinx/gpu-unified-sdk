/*
 * Copyright (C) 2024-2026 Advanced Micro Devices, Inc.  All rights reserved.
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


#include <iostream>
#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


/**
 * \brief Long data type (64-bit integer) OpenCL example.
 * \details An example to calculate, for an image:
 *          - the sum of the squares of the pixel values,
 *          - the sum of the pixel values.
 *          Makes use of the long data type and 64-bit atomics.
 *          The main calculation code is in an OpenCL kernel which is executed on a GPU device.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();

	/* Checking 64-bit integer atomics extension support. */
	if (!ocl::isExtensionSupported(device, "cl_khr_int64_base_atomics")) {
		std::cerr << "The device does not support the cl_khr_int64_base_atomics extension."
				<< std::endl;
		return 1;
	}

	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/integer_64_bit.cl");
	const ocl::Kernel kernel = program.createKernel("long_vectors");

	/* Load 24-bits per pixel RGB data from a bitmap. */
	int width, height;
	unsigned char * loadedRGBData = nullptr;
	loadFromBitmap("assets/input.bmp", &width, &height, &loadedRGBData);

	const int64_t bufferSizeChar = width * height * sizeof(cl_uchar);
	const int64_t bufferSizeLong = sizeof(cl_ulong);

	const ocl::Mem imagePixelsMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSizeChar);
	const ocl::Mem squareMem = context.createBuffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
			bufferSizeLong);
	const ocl::Mem sumMem = context.createBuffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
			bufferSizeLong);

	{
		const ocl::EnqueueBufferRet<cl_uchar> imagePixels = commandQueue.enqueueMapBuffer<cl_uchar>(
				imagePixelsMem, true, CL_MAP_WRITE, 0, bufferSizeChar, false);
		const ocl::EnqueueBufferRet<cl_ulong> square = commandQueue.enqueueMapBuffer<cl_ulong>(
				squareMem, true, CL_MAP_WRITE, 0, bufferSizeLong, false);
		const ocl::EnqueueBufferRet<cl_ulong> sum = commandQueue.enqueueMapBuffer<cl_ulong>(
				sumMem, true, CL_MAP_WRITE, 0, bufferSizeLong, false);

		/* Convert 24-bits per pixel RGB into 8-bits per pixel luminance data. */
		RGBToLuminance(loadedRGBData, imagePixels.memory.data(), width, height);

		/* Ensure the accumulators are initialized to zero. */
		*square.memory.data() = 0;
		*sum.memory.data() = 0;
	}

	delete[] loadedRGBData;

	kernel.setArg<cl_mem>(0, imagePixelsMem);
	kernel.setArg<cl_mem>(1, squareMem);
	kernel.setArg<cl_mem>(2, sumMem);

	/*
	 * Each instance of the kernel operates on an 8 * 1 portion of the image.
	 * Therefore, the global work size must be (width * height) / 8.
	 */
	const size_t globalWorkSize[1] = { static_cast<size_t>((width * height) / 8) };

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 1,
			nullptr, globalWorkSize, nullptr, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	{
		const ocl::EnqueueBufferRet<cl_ulong> square = commandQueue.enqueueMapBuffer<cl_ulong>(
				squareMem, true, CL_MAP_READ, 0, bufferSizeLong, false);
		const ocl::EnqueueBufferRet<cl_ulong> sum = commandQueue.enqueueMapBuffer<cl_ulong>(
				sumMem, true, CL_MAP_READ, 0, bufferSizeLong, false);

		std::cout << "Square of the pixel values = " << *square.memory.data() << "\n";
		std::cout << "Sum of the pixel values = " << *sum.memory.data() << std::endl;
	}

	return 0;
}
