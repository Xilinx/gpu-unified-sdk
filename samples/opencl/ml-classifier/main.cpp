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


#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


/**
 * \brief Simple ML image classifier using OpenCL.
 * \details Loads a bitmap, computes the average intensity using an OpenCL kernel, and applies a
 *          linear classifier to label the image as "Bright" or "Dark". The input image is loaded
 *          from assets/input.bmp. The output image is stored in output.bmp, as a grayscale copy
 *          of the input for verification.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/ml_classifier.cl");
	const ocl::Kernel kernel = program.createKernel("compute_intensity_sum");

	int width, height;
	unsigned char * imageData = nullptr;
	loadFromBitmap("assets/input.bmp", &width, &height, &imageData);

	const int64_t bufferSize = width * height * sizeof(cl_uchar);
	const size_t workgroupsX = (width + 15) / 16;
	const size_t workgroupsY = height;
	const int64_t outputSize = workgroupsX * workgroupsY * sizeof(cl_float);

	const ocl::Mem imageMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem partialSumsMem = context.createBuffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
			outputSize);

	/* Initialize partialSums buffer to zero. */
	{
		const ocl::EnqueueBufferRet<cl_float> partialSums = commandQueue.enqueueMapBuffer<cl_float>(
				partialSumsMem, true, CL_MAP_WRITE, 0, outputSize, false);
		for (size_t i = 0; i < workgroupsX * workgroupsY; i++) {
			partialSums.memory.data()[i] = 0.0f;
		}
	}

	{
		const ocl::EnqueueBufferRet<cl_uchar> luminance = commandQueue.enqueueMapBuffer<cl_uchar>(
				imageMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		RGBToLuminance(imageData, luminance.memory.data(), width, height);

		std::cout << "Sample Luminance Values: ";
		for (int i = 0; i < std::min(10, width * height); i++) {
			std::cout << static_cast<int>(luminance.memory.data()[i]) << " ";
		}
		std::cout << std::endl;
	}
	delete[] imageData;

	kernel.setArg<cl_mem>(0, imageMem);
	kernel.setArg<cl_int>(1, width);
	kernel.setArg<cl_int>(2, height);
	kernel.setArg<cl_mem>(3, partialSumsMem);

	const size_t globalWorkSize[2] = { workgroupsX, workgroupsY };
	const size_t localWorkSize[2] = { 16, 1 };

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorkSize, localWorkSize, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	float averageIntensity = 0.0f;
	{
		const ocl::EnqueueBufferRet<cl_float> partialSums = commandQueue.enqueueMapBuffer<cl_float>(
				partialSumsMem, true, CL_MAP_READ, 0, outputSize, false);

		float totalSum = 0.0f;
		for (size_t i = 0; i < workgroupsX * workgroupsY; i++) {
			totalSum += partialSums.memory.data()[i];
		}
		averageIntensity = totalSum / (width * height);
		std::cout << "Total Sum: " << totalSum << std::endl;
		std::cout << "Image Size: " << width * height << std::endl;
		std::cout << "Average Intensity: " << averageIntensity << std::endl;
	}

	/* Simple linear classifier: score = w * feature + b. */
	const float weight = 1.0f;
	const float bias = -128.0f;
	const float score = weight * averageIntensity + bias;
	const std::string classification = (score > 0) ? "Bright" : "Dark";
	std::cout << "Classification: " << classification << std::endl;

	/* Create output image (direct copy of input luminance). */
	{
		const ocl::EnqueueBufferRet<cl_uchar> output = commandQueue.enqueueMapBuffer<cl_uchar>(
				imageMem, true, CL_MAP_READ, 0, bufferSize, false);

		std::vector<unsigned char> rgbOut(width * height * 3);
		luminanceToRGB(output.memory.data(), rgbOut.data(), width, height);
		saveToBitmap("output.bmp", width, height, rgbOut.data());
	}

	return 0;
}
