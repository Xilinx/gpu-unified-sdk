
#include <vector>

#include <Common/Color.hpp>
#include <OpenCL/CLUtils.hpp>


int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/image_scaling.cl");
	const ocl::Kernel kernel = program.createKernel("image_scaling");

	ocl::printSupported2DImageFormats(context);

	/* The scaling factor to use when resizing the image. */
	const int scaleFactor = 8;

	/* Load the input image data. */
	unsigned char* inputImage = nullptr;
	int width, height;
	loadFromBitmap("assets/input.bmp", &width, &height, &inputImage);

	/*
	 * Calculate the width and height of the new image.
	 * Used to allocate the correct amount of output memory and the number of kernels to use.
	 */
	int newWidth = width * scaleFactor;
	int newHeight = height * scaleFactor;

	/* [Allocate image objects] */
	/*
	 * Specify the format of the image.
	 * The bitmap image we are using is RGB888, which is not a supported OpenCL image format.
	 * We will use RGBA8888 and add an empty alpha channel.
	 */
	const cl_image_format format {
		.image_channel_order = CL_RGBA,
		.image_channel_data_type = CL_UNORM_INT8,
	};

	const ocl::Mem readMem = context.createImage(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			format,
			cl_image_desc {
				.image_type = CL_MEM_OBJECT_IMAGE2D,
				.image_width = size_t(width),
				.image_height = size_t(height),
			},
			nullptr);

	const ocl::Mem writeMem = context.createImage(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			format,
			cl_image_desc {
				.image_type = CL_MEM_OBJECT_IMAGE2D,
				.image_width = size_t(newWidth),
				.image_height = size_t(newHeight),
			},
			nullptr);

	/* [Allocate image objects] */

	/* [Map image objects to host pointers] */
	/*
	 * Like with memory buffers, we now map the allocated memory to a host side pointer.
	 * Unlike buffers, we must specify origin coordinates, width and height for the region of the image we wish to map.
	 */
	size_t origin[3] = {0, 0, 0};
	size_t region[3] = {static_cast<size_t>(width), static_cast<size_t>(height), 1};

	/*
	 * clEnqueueMapImage also returns the rowPitch; the width of the mapped region in bytes.
	 * If the image format is not known, this is required information when accessing the image object as a normal array.
	 * The number of bytes per pixel can vary with the image format being used,
	 * this affects the offset into the array for a given coordinate.
	 * In our case the image format is fixed as RGBA8888 so we don't need to worry about the rowPitch.
	 */
	size_t rowPitch;

	{
		ocl::EnqueueImageRet<uint8_t> inputImageRGBA = commandQueue.enqueueMapImage<uint8_t>(
				readMem, true, CL_MAP_WRITE, origin, region, false);

		/* Convert the input data from RGB to RGBA (moves it to the OpenCL allocated memory at the same time). */
		RGBToRGBA(inputImage, inputImageRGBA.memory.data(), width, height);
	}

	delete[] inputImage;

	/*
	 * Calculate the normalization factor for the image coordinates.
	 * By using normalized coordinates we don't have to manually map the destination coordinates to the source coordinates.
	 */
	cl_float widthNormalizationFactor = 1.0f / newWidth;
	cl_float heightNormalizationFactor = 1.0f / newHeight;

	/* Setup the kernel arguments. */
	kernel.setArg<cl_mem>(0, readMem);
	kernel.setArg<cl_mem>(1, writeMem);
	kernel.setArg<cl_float>(2, widthNormalizationFactor);
	kernel.setArg<cl_float>(3, heightNormalizationFactor);

	/*
	 * Set the kernel work size. Each kernel operates on one pixel of the ouput image.
	 * Therefore, we need newWidth * newHeight kernel instances.
	 * We are using two work dimensions because it maps nicely onto the coordinates of the image.
	 * With one dimension we would have to derive the y coordinate from the x coordinate in the kernel.
	 */
	const int workDimensions = 2;
	const size_t globalWorkSize[workDimensions] = {
		static_cast<size_t>(newWidth),
		static_cast<size_t>(newHeight),
	};

	/* An event to associate with the kernel. Allows us to retrieve profiling information later. */
	/* Enqueue the kernel. */
	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, workDimensions,
			nullptr, globalWorkSize, nullptr,
			0, nullptr, true);

	/* Wait for kernel execution completion. */
	CHECKCL(clFinish(commandQueue))

	/* Print the profiling information for the event. */
	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	const size_t newRegion[3] = {
		static_cast<size_t>(newWidth),
		static_cast<size_t>(newHeight),
		1,
	};

	{
		const ocl::EnqueueImageRet<uint8_t> outputImage = commandQueue.enqueueMapImage<uint8_t>(
				writeMem, true, CL_MAP_READ, origin, newRegion, false);

		std::vector<uint8_t> outputImageRGB(newWidth * newHeight * 3);
		RGBAToRGB(outputImage.memory.data(), outputImageRGB.data(), newWidth, newHeight);

		saveToBitmap("output.bmp", newWidth, newHeight, outputImageRGB.data());
	}

	return 0;
}
