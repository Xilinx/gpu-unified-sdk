
#include <OpenCL/CLUtils.hpp>


/**
 * \brief Simple template OpenCL sample.
 * \details The basic code to run a kernel with no arguments.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/opencl_template.cl");
	const ocl::Kernel kernel = program.createKernel("template");

	/*
	 * Add code here to set up memory/data, for example:
	 * - Create memory buffers.
	 * - Initialise the input data.
	 * - Set up kernel arguments.
	 */

	const int workDimensions = 1;
	const size_t globalWorkSize[workDimensions] = { 1 };

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, workDimensions,
			nullptr, globalWorkSize, nullptr, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	/* Add code here to retrieve results of the kernel execution. */

	return 0;
}
