
#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


/**
 * \brief Convolution filter OpenCL sample with custom kernel.
 * \details Loads a bitmap and applies a user-defined 3x3 or 5x5 convolution filter using an
 *          OpenCL kernel. The input image is loaded from assets/input.bmp. The output filtered
 *          image is stored in output.bmp. The convolution kernel is passed as a float buffer,
 *          with its size (3 or 5) specified.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/convolution_filter.cl");
	const ocl::Kernel kernel = program.createKernel("convolution");

	int width, height;
	unsigned char * imageData = nullptr;
	loadFromBitmap("assets/input.bmp", &width, &height, &imageData);

	const int64_t bufferSize = width * height * sizeof(cl_uchar);

	/* Define a 3x3 kernel (Gaussian); the user can modify this to a 5x5 kernel. */
	const cl_int kernelSize = 3;
	float kernelData[5 * 5] = { 0 };
	if (kernelSize == 3) {
		const float gaussianKernel[3][3] = {
			{ 1.0f, 2.0f, 1.0f },
			{ 2.0f, 4.0f, 2.0f },
			{ 1.0f, 2.0f, 1.0f },
		};
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				kernelData[i * 3 + j] = gaussianKernel[i][j] / 16.0f;
			}
		}
	} else {
		const float gaussianKernel[5][5] = {
			{ 1.0f, 4.0f, 6.0f, 4.0f, 1.0f },
			{ 4.0f, 16.0f, 24.0f, 16.0f, 4.0f },
			{ 6.0f, 24.0f, 36.0f, 24.0f, 6.0f },
			{ 4.0f, 16.0f, 24.0f, 16.0f, 4.0f },
			{ 1.0f, 4.0f, 6.0f, 4.0f, 1.0f },
		};
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				kernelData[i * 5 + j] = gaussianKernel[i][j] / 256.0f;
			}
		}
	}

	const ocl::Mem readMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem writeMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem kernelMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			kernelSize * kernelSize * sizeof(cl_float));

	{
		const ocl::EnqueueBufferRet<cl_uchar> luminance = commandQueue.enqueueMapBuffer<cl_uchar>(
				readMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		RGBToLuminance(imageData, luminance.memory.data(), width, height);
	}
	delete[] imageData;

	{
		const ocl::EnqueueBufferRet<cl_float> kernelBuffer = commandQueue.enqueueMapBuffer<cl_float>(
				kernelMem, true, CL_MAP_WRITE, 0, kernelSize * kernelSize * sizeof(cl_float), false);
		for (int i = 0; i < kernelSize * kernelSize; i++) {
			kernelBuffer.memory.data()[i] = kernelData[i];
		}
	}

	kernel.setArg<cl_mem>(0, readMem);
	kernel.setArg<cl_int>(1, width);
	kernel.setArg<cl_mem>(2, writeMem);
	kernel.setArg<cl_mem>(3, kernelMem);
	kernel.setArg<cl_int>(4, kernelSize);

	const size_t globalWorkSize[2] = {
		static_cast<size_t>(width / 16),
		static_cast<size_t>(height),
	};

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 2,
			nullptr, globalWorkSize, nullptr, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	{
		const ocl::EnqueueBufferRet<cl_uchar> output = commandQueue.enqueueMapBuffer<cl_uchar>(
				writeMem, true, CL_MAP_READ, 0, bufferSize, false);

		std::vector<unsigned char> rgbOut(width * height * 3);
		luminanceToRGB(output.memory.data(), rgbOut.data(), width, height);
		saveToBitmap("output.bmp", width, height, rgbOut.data());
	}

	return 0;
}
