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


#include <Common/Color.hpp>
#include <Common/Geometry.hpp>
#include <Common/PngUtils.hpp>
#include <OpenCL/CLUtils.hpp>


int main(int argc, char ** argv)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/gaussian_filter.cl");
	const ocl::Kernel kernel = program.createKernel("gaussian");

	const Image image = ::loadPng("assets/input.png",
			PngLoadParams{.colorType = PngColorType::Rgb});

	const int bufferSize = image.size.width * image.size.height * sizeof(cl_uchar);
	const ocl::Mem readMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem writeMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);

	{
		ocl::EnqueueBufferRet<cl_uchar> luminance = commandQueue.enqueueMapBuffer<cl_uchar>(
				readMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		RGBToLuminance(image.data.get(), luminance.memory.data(),
				image.size.width, image.size.height);
	}

	kernel.setArg<cl_mem>(0, readMem);
	kernel.setArg<cl_int>(1, image.size.width);
	kernel.setArg<cl_mem>(2, writeMem);

	const size_t globalWorkSize[2] = {
		static_cast<size_t>(image.size.width / 16),
		static_cast<size_t>(image.size.height / 1),
	};

	ocl::Event event = 	commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorkSize, nullptr,
			0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	{
		ocl::EnqueueBufferRet<cl_uchar> output = commandQueue.enqueueMapBuffer<cl_uchar>(
				writeMem, true, CL_MAP_READ, 0, bufferSize, false);
		Image outputImage = Image::create(PixelFormat::RGB888, image.size);
		luminanceToRGB(output.memory.data(), outputImage.data.get(),
				image.size.width, image.size.height);
		dumpPng("output.png", outputImage, PngDumpParams{.colorType = PngColorType::Rgb});
	}

	return 0;
}
