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


#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


/**
 * \brief Simple FIR filter OpenCL sample.
 * \details A sample which loads an image from assets/input.bmp and then passes it to the GPU.
 *          An OpenCL kernel applies FIR filtering on the data and the output image data is
 *          stored in output.bmp on the target.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/fir_filter.cl");
	const ocl::Kernel kernel = program.createKernel("fir_float");

	/* Load 24-bits per pixel RGB data from a bitmap. */
	int width, height;
	unsigned char * loadedRGBData = nullptr;
	loadFromBitmap("assets/input.bmp", &width, &height, &loadedRGBData);

	/* Convert 24-bits per pixel RGB into 8-bits per pixel luminance data. */
	std::vector<unsigned char> inputLuminance(width * height);
	RGBToLuminance(loadedRGBData, inputLuminance.data(), width, height);
	delete[] loadedRGBData;

	/* All buffers are the size of the image data as floats. */
	const int64_t bufferSize = width * height * sizeof(cl_float);

	const ocl::Mem readMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem writeMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);

	/* Convert luminance data into float (a real world application would use real float data). */
	{
		const ocl::EnqueueBufferRet<cl_float> input = commandQueue.enqueueMapBuffer<cl_float>(
				readMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		for (int i = 0; i < width * height; i++) {
			input.memory.data()[i] = static_cast<float>(inputLuminance[i]) / 255.0f;
		}
	}

	kernel.setArg<cl_mem>(0, readMem);
	kernel.setArg<cl_mem>(1, writeMem);
	kernel.setArg<cl_int>(2, width);

	/*
	 * Each instance of the kernel operates on a 4 * 1 portion of the image.
	 * Therefore, the global work size must be width / 4 by height / 1 work items.
	 */
	const size_t globalWorkSize[2] = {
		static_cast<size_t>(width / 4),
		static_cast<size_t>(height / 1),
	};

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorkSize, nullptr, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	{
		const ocl::EnqueueBufferRet<cl_float> output = commandQueue.enqueueMapBuffer<cl_float>(
				writeMem, true, CL_MAP_READ, 0, bufferSize, false);

		/* Convert the float output to unsigned char for saving to bitmap. */
		std::vector<unsigned char> outputData(width * height);
		for (int i = 0; i < width * height; i++) {
			outputData[i] = static_cast<unsigned char>(output.memory.data()[i] * 255.0f);
		}

		std::vector<unsigned char> rgbOut(width * height * 3);
		luminanceToRGB(outputData.data(), rgbOut.data(), width, height);
		saveToBitmap("output.bmp", width, height, rgbOut.data());
	}

	return 0;
}
