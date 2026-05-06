
#include <Common/Color.hpp>
#include <Common/Folders.hpp>
#include <Common/Geometry.hpp>
#include <Common/PngUtils.hpp>
#include <OpenCL/CLUtils.hpp>


int main(int argc, char ** argv)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "gaussian_filter.cl");
	const ocl::Kernel kernel = program.createKernel("gaussian");

	const Image image = ::loadPng(Folders::get().imagesDir / "input.png",
			PngLoadParams{.colorType = PngColorType::Rgb});

	const int bufferSize = image.size.width * image.size.height * sizeof(cl_uchar);
	const ocl::Mem readMem = ocl::Mem::createBuffer(context,
			CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, bufferSize);
	const ocl::Mem writeMem = ocl::Mem::createBuffer(context,
			CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR, bufferSize);

	{
		ocl::MappedBuffer<cl_uchar> luminance = commandQueue.enqueueMapBuffer<cl_uchar>(
				readMem, true, CL_MAP_WRITE, 0, bufferSize);
		RGBToLuminance(image.data.get(), luminance.data(), image.size.width, image.size.height);
	}

	kernel.setArg<cl_mem>(0, readMem);
	kernel.setArg<cl_int>(1, image.size.width);
	kernel.setArg<cl_mem>(2, writeMem);

	cl_event event = nullptr;
	const size_t globalWorkSize[2] = {
		static_cast<size_t>(image.size.width / 16),
		static_cast<size_t>(image.size.height / 1),
	};

	CHECKCL(clEnqueueNDRangeKernel(commandQueue, kernel, 2, nullptr, globalWorkSize, nullptr,
			0, nullptr, &event))

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	CHECKCL(clReleaseEvent(event))

	{
		ocl::MappedBuffer<cl_uchar> output = commandQueue.enqueueMapBuffer<cl_uchar>(
				writeMem, true, CL_MAP_READ, 0, bufferSize);
		Image outputImage = Image::create(PixelFormat::RGB888, image.size);
		luminanceToRGB(output.data(), outputImage.data.get(), image.size.width, image.size.height);
		dumpPng("output.png", outputImage, {});
	}

	return 0;
}
