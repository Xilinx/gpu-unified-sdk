
#include <OpenCL/CLUtils.hpp>


/**
 * \brief Basic integer array addition implemented in OpenCL.
 * \details A sample which shows how to add two integer arrays and store the result in a third array.
 *          The main calculation code is in an OpenCL kernel which is executed on a GPU device.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/hello_world_opencl.cl");
	const ocl::Kernel kernel = program.createKernel("hello_world_opencl");

	/* Number of elements in the arrays of input and output data. */
	const cl_int arraySize = 1000000;
	const int64_t bufferSize = arraySize * sizeof(cl_int);

	/*
	 * Ask the OpenCL implementation to allocate buffers for the data.
	 * We ask the OpenCL implementation to allocate memory rather than allocating
	 * it on the CPU to avoid having to copy the data later.
	 */
	const ocl::Mem inputAMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem inputBMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem outputMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);

	/* Map the input buffers and initialize them. */
	{
		const ocl::EnqueueBufferRet<cl_int> inputA = commandQueue.enqueueMapBuffer<cl_int>(
				inputAMem, true, CL_MAP_WRITE, 0, bufferSize, false);
		const ocl::EnqueueBufferRet<cl_int> inputB = commandQueue.enqueueMapBuffer<cl_int>(
				inputBMem, true, CL_MAP_WRITE, 0, bufferSize, false);

		for (int i = 0; i < arraySize; i++) {
			inputA.memory.data()[i] = i;
			inputB.memory.data()[i] = i;
		}
	}

	kernel.setArg<cl_mem>(0, inputAMem);
	kernel.setArg<cl_mem>(1, inputBMem);
	kernel.setArg<cl_mem>(2, outputMem);

	/*
	 * Each instance of our OpenCL kernel operates on a single element of each array so the number
	 * of instances needed is the number of elements in the array.
	 */
	const size_t globalWorkSize[1] = { static_cast<size_t>(arraySize) };

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 1,
			nullptr, globalWorkSize, nullptr, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	/* Map the output buffer (results could be inspected here). */
	{
		const ocl::EnqueueBufferRet<cl_int> output = commandQueue.enqueueMapBuffer<cl_int>(
				outputMem, true, CL_MAP_READ, 0, bufferSize, false);
		(void)output;
	}

	return 0;
}
