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


#include <cmath>
#include <cstdlib>
#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


/**
 * \brief Simple Sobel filter OpenCL sample.
 * \details A sample which loads a bitmap and then passes it to the GPU. An OpenCL kernel which
 *          does Sobel filtering is then run on the data. The gradients of the image in x and y
 *          directions are returned by the GPU and are combined on the CPU to form the filtered
 *          data. The input image is loaded from assets/input.bmp (its width must be divisible by
 *          16). The output gradients in X and Y, as well as the combined gradient image, are
 *          stored in output-dX.bmp, output-dY.bmp and output.bmp respectively.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/sobel_filter.cl");
	const ocl::Kernel kernel = program.createKernel("sobel");

	/* Load 24-bits per pixel RGB data from a bitmap. */
	int width, height;
	unsigned char * imageData = nullptr;
	loadFromBitmap("assets/input.bmp", &width, &height, &imageData);

	const int64_t bufferSize = width * height * sizeof(cl_uchar);

	const ocl::Mem readMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem dxMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem dyMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);

	/* Convert 24-bits per pixel RGB into 8-bits per pixel luminance data. */
	{
		const ocl::EnqueueBufferRet<cl_uchar> luminance = commandQueue.enqueueMapBuffer<cl_uchar>(
				readMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		RGBToLuminance(imageData, luminance.memory.data(), width, height);
	}
	delete[] imageData;

	kernel.setArg<cl_mem>(0, readMem);
	kernel.setArg<cl_int>(1, width);
	kernel.setArg<cl_mem>(2, dxMem);
	kernel.setArg<cl_mem>(3, dyMem);

	/*
	 * Each instance of the kernel operates on a 16 * 1 portion of the image.
	 * Therefore, the global work size must be width / 16 by height / 1 work items.
	 */
	const size_t globalWorkSize[2] = {
		static_cast<size_t>(width / 16),
		static_cast<size_t>(height / 1),
	};

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorkSize, nullptr, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	/* Take the absolute values of the gradients to visualise the data. */
	std::vector<unsigned char> absDX(width * height);
	std::vector<unsigned char> absDY(width * height);
	{
		const ocl::EnqueueBufferRet<cl_char> outputDx = commandQueue.enqueueMapBuffer<cl_char>(
				dxMem, true, CL_MAP_READ, 0, bufferSize, false);
		const ocl::EnqueueBufferRet<cl_char> outputDy = commandQueue.enqueueMapBuffer<cl_char>(
				dyMem, true, CL_MAP_READ, 0, bufferSize, false);

		for (int i = 0; i < width * height; i++) {
			absDX[i] = std::abs(outputDx.memory.data()[i]);
			absDY[i] = std::abs(outputDy.memory.data()[i]);
		}
	}

	/* Convert the two output luminance arrays to RGB and save them out to files. */
	std::vector<unsigned char> rgbOut(width * height * 3);
	luminanceToRGB(absDX.data(), rgbOut.data(), width, height);
	saveToBitmap("output-dX.bmp", width, height, rgbOut.data());

	luminanceToRGB(absDY.data(), rgbOut.data(), width, height);
	saveToBitmap("output-dY.bmp", width, height, rgbOut.data());

	/* Calculate the total gradient of the image, convert it to RGB and store it out to a file. */
	std::vector<unsigned char> totalOutput(width * height);
	for (int index = 0; index < width * height; index++) {
		totalOutput[index] = static_cast<unsigned char>(
				std::sqrt(std::pow(absDX[index], 2) + std::pow(absDY[index], 2)));
	}
	luminanceToRGB(totalOutput.data(), rgbOut.data(), width, height);
	saveToBitmap("output.bmp", width, height, rgbOut.data());

	return 0;
}
