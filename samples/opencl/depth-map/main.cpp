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
 * \brief Obstacle detection using stereo disparity mapping with OpenCL.
 * \details Loads two stereo images (left.bmp, right.bmp), computes a disparity map using
 *          block-matching, and saves a grayscale depth map as output.bmp. Brighter pixels
 *          indicate closer objects.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/obstacle_detection.cl");
	const ocl::Kernel kernel = program.createKernel("compute_disparity");

	int width, height, width2, height2;
	unsigned char * leftImageData = nullptr;
	unsigned char * rightImageData = nullptr;
	loadFromBitmap("assets/left.bmp", &width, &height, &leftImageData);
	loadFromBitmap("assets/right.bmp", &width2, &height2, &rightImageData);
	if (width != width2 || height != height2) {
		delete[] leftImageData;
		delete[] rightImageData;
		std::cerr << "Left and right images must have identical dimensions." << std::endl;
		return 1;
	}

	const int64_t bufferSize = width * height * sizeof(cl_uchar);

	const ocl::Mem leftMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem rightMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem disparityMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);

	{
		const ocl::EnqueueBufferRet<cl_uchar> leftLuminance = commandQueue.enqueueMapBuffer<cl_uchar>(
				leftMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		const ocl::EnqueueBufferRet<cl_uchar> rightLuminance = commandQueue.enqueueMapBuffer<cl_uchar>(
				rightMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		RGBToLuminance(leftImageData, leftLuminance.memory.data(), width, height);
		RGBToLuminance(rightImageData, rightLuminance.memory.data(), width, height);
	}
	delete[] leftImageData;
	delete[] rightImageData;

	const cl_int maxDisparity = 64;
	const cl_int windowSize = 5;

	kernel.setArg<cl_mem>(0, leftMem);
	kernel.setArg<cl_mem>(1, rightMem);
	kernel.setArg<cl_int>(2, width);
	kernel.setArg<cl_int>(3, height);
	kernel.setArg<cl_mem>(4, disparityMem);
	kernel.setArg<cl_int>(5, maxDisparity);
	kernel.setArg<cl_int>(6, windowSize);

	const size_t globalWorkSize[2] = {
		static_cast<size_t>(width / 16),
		static_cast<size_t>(height),
	};
	const size_t localWorkSize[2] = { 16, 1 };

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorkSize, localWorkSize, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	{
		const ocl::EnqueueBufferRet<cl_uchar> disparityMap = commandQueue.enqueueMapBuffer<cl_uchar>(
				disparityMem, true, CL_MAP_READ, 0, bufferSize, false);

		/* Simple obstacle detection: count pixels with high disparity (close objects). */
		int obstacleCount = 0;
		const cl_uchar disparityThreshold = 32;
		for (int i = 0; i < width * height; i++) {
			if (disparityMap.memory.data()[i] > disparityThreshold) {
				obstacleCount++;
			}
		}
		std::cout << "Obstacle Pixels (Disparity > " << static_cast<int>(disparityThreshold)
				<< "): " << obstacleCount << std::endl;
		std::cout << "Percentage of Image: " << (obstacleCount * 100.0f / (width * height))
				<< "%" << std::endl;

		std::vector<unsigned char> rgbOut(width * height * 3);
		luminanceToRGB(disparityMap.memory.data(), rgbOut.data(), width, height);
		saveToBitmap("output.bmp", width, height, rgbOut.data());
	}

	return 0;
}
