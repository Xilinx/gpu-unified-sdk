
#include <iostream>

#include <OpenCL/CLUtils.hpp>


/**
 * \brief Basic integer array addition implemented in OpenCL.
 * \details A sample which shows how to add two integer arrays and store the result in a third array.
 *          The main calculation code is in an OpenCL kernel which is executed on a GPU device.
 *          The OpenCL kernel makes use of vectors for improved performance.
 * \return The exit code of the application, non-zero if a problem occurred.
 */
int main(void)
{
	const ocl::Context context = ocl::Context::create();
	const cl_device_id device = context.findDevice();
	const ocl::CommandQueue commandQueue = context.createCommandQueue(device);
	const ocl::Program program = context.createProgram(device, "assets/vector_hello_world.cl");
	const ocl::Kernel kernel = program.createKernel("hello_world_vector");

	/*
	 * Query the device to find out its preferred integer vector width.
	 * Although we are only printing the value here, it can be used to select between
	 * different versions of a kernel.
	 */
	cl_uint integerVectorWidth = 0;
	CHECKCL(clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,
			sizeof(cl_uint), &integerVectorWidth, nullptr))
	std::cout << "Preferred vector width for integers: " << integerVectorWidth << std::endl;

	const cl_int arraySize = 1000000;
	const int64_t bufferSize = arraySize * sizeof(cl_int);

	const ocl::Mem inputAMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem inputBMem = context.createBuffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);
	const ocl::Mem outputMem = context.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
			bufferSize);

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
	 * Each instance of our OpenCL kernel now operates on 4 elements of each array so the number
	 * of instances needed is the number of elements in the array divided by 4.
	 */
	const size_t globalWorkSize[1] = { arraySize / static_cast<size_t>(4) };

	ocl::Event event = commandQueue.enqueueNDRangeKernel(kernel, 1,
			nullptr, globalWorkSize, nullptr, 0, nullptr, true);

	CHECKCL(clFinish(commandQueue))

	ocl::printProfilingInfo(event);

	event = ocl::Event::null();

	{
		const ocl::EnqueueBufferRet<cl_int> output = commandQueue.enqueueMapBuffer<cl_int>(
				outputMem, true, CL_MAP_READ, 0, bufferSize, false);
		(void)output;
	}

	return 0;
}
