
#include <chrono>
#include <iostream>
#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


/**
 * \brief ADAS lane detection visualization using OpenCL.
 * \details Loads a road image, applies Sobel edge detection to highlight lane lines, and measures
 *          performance on the GPU.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/adas_lane_detection.cl");
	const ocl::Kernel kernel = program.createKernel("sobel_edge_detection");

	int width, height;
	unsigned char * inputImageData = nullptr;
	loadFromBitmap("assets/input.bmp", &width, &height, &inputImageData);
	if (width != 1024 || height != 1024) {
		std::cerr << "Input image must be 1024x1024. Got " << width << "x" << height << "."
				<< std::endl;
		delete[] inputImageData;
		return 1;
	}

	const int64_t bufferSize = width * height * sizeof(cl_uchar);

	/*
	 * The original sample copies the raw (RGB) buffer into a luminance-sized buffer. We reproduce
	 * that behaviour for parity: the first width*height bytes of the RGB data are treated as
	 * luminance by the kernel.
	 */
	const ocl::Mem readMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem writeMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);

	{
		const ocl::EnqueueBufferRet<cl_uchar> input = commandQueue.enqueueMapBuffer<cl_uchar>(
				readMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		for (int64_t i = 0; i < bufferSize; i++) {
			input.memory.data()[i] = inputImageData[i];
		}
	}
	delete[] inputImageData;

	kernel.setArg<cl_mem>(0, readMem);
	kernel.setArg<cl_int>(1, width);
	kernel.setArg<cl_int>(2, height);
	kernel.setArg<cl_mem>(3, writeMem);

	const size_t globalWorkSize[2] = {
		static_cast<size_t>(width),
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
	std::cout << "Total Time: " << duration.count() << " ms" << std::endl;
	std::cout << "Pixels Processed per Second: "
			<< (width * height) / (duration.count() / 1000.0) << std::endl;

	{
		const ocl::EnqueueBufferRet<cl_uchar> output = commandQueue.enqueueMapBuffer<cl_uchar>(
				writeMem, true, CL_MAP_READ, 0, bufferSize, false);

		std::vector<unsigned char> rgbOut(width * height * 3);
		luminanceToRGB(output.memory.data(), rgbOut.data(), width, height);
		saveToBitmap("output.bmp", width, height, rgbOut.data());
	}

	return 0;
}
