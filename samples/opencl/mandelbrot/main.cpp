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


#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


/**
 * \brief A sample which generates Mandelbrot data for a given data size.
 * \details For a given height and width, the sample will test each pixel
 *          to work out how many iterations it takes to determine if the
 *          value is not part of the Mandelbrot set. This data is output
 *          as a greyscale bitmap image. White pixels could not be ruled out
 *          of the Mandelbrot set in the number of iterations used.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/mandelbrot.cl");
	const ocl::Kernel kernel = program.createKernel("mandelbrot");

	/* Width and height of the Mandelbrot data you want to be produced. */
	cl_int width = 4096;
	cl_int height = 3280;

	/* The output buffer is the size of the Mandelbrot data. */
	size_t bufferSize = width * height * sizeof(cl_uchar);

	/* Create an output buffer for final data. */
	const ocl::Mem writeMem = context.createBuffer(
			CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, bufferSize);

	/* Setup the kernel arguments. */
	kernel.setArg<cl_mem>(0, writeMem);
	kernel.setArg<cl_int>(1, width);
	kernel.setArg<cl_int>(2, height);

	/*
	 * The OpenCL kernel calculates four pixels at a time (all in the same row).
	 * Therefore, we only need to run the kernel width / 4 times in the x dimension.
	 */
	const size_t globalWorksize[2] = {
		static_cast<size_t>(width / 4),
		static_cast<size_t>(height),
	};

	/* Enqueue the kernel */
	/* An event to associate with the Kernel. Allows us to retreive profiling information later. */
	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorksize, nullptr, 0, NULL, true);

	/* Wait for kernel execution completion. */
	CHECKCL(clFinish(commandQueue))

	/* Print the profiling information for the event. */
	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	{
		/* Map the output memory to a host side pointer. */
		const ocl::EnqueueBufferRet<cl_uchar> output = commandQueue.enqueueMapBuffer<cl_uchar>(
				writeMem, true, CL_MAP_READ, 0, bufferSize, false);

		/* Convert the output luminance array to RGB and save it out to a file. */
		std::vector<uint8_t> rgbOut(width * height * 3);
		luminanceToRGB(output.memory.data(), rgbOut.data(), width, height);
		saveToBitmap("output.bmp", width, height, rgbOut.data());
	}

	return 0;
}
