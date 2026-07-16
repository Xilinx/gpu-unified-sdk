/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#include "CLUtils.hpp"

#include <CL/cl_ext.h>

#include <Common/FileUtils.hpp>




namespace ocl {




std::string_view errorCodeToString(const cl_int error) noexcept
{
#define ERROR_CODE(x) case x: return #x;

	switch (error) {
	ERROR_CODE(CL_SUCCESS)
	ERROR_CODE(CL_DEVICE_NOT_FOUND)
	ERROR_CODE(CL_DEVICE_NOT_AVAILABLE)
	ERROR_CODE(CL_COMPILER_NOT_AVAILABLE)
	ERROR_CODE(CL_MEM_OBJECT_ALLOCATION_FAILURE)
	ERROR_CODE(CL_OUT_OF_RESOURCES)
	ERROR_CODE(CL_OUT_OF_HOST_MEMORY)
	ERROR_CODE(CL_PROFILING_INFO_NOT_AVAILABLE)
	ERROR_CODE(CL_MEM_COPY_OVERLAP)
	ERROR_CODE(CL_IMAGE_FORMAT_MISMATCH)
	ERROR_CODE(CL_IMAGE_FORMAT_NOT_SUPPORTED)
	ERROR_CODE(CL_BUILD_PROGRAM_FAILURE)
	ERROR_CODE(CL_MAP_FAILURE)
	ERROR_CODE(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST)
	ERROR_CODE(CL_INVALID_VALUE)
	ERROR_CODE(CL_INVALID_DEVICE_TYPE)
	ERROR_CODE(CL_INVALID_PLATFORM)
	ERROR_CODE(CL_INVALID_DEVICE)
	ERROR_CODE(CL_INVALID_CONTEXT)
	ERROR_CODE(CL_INVALID_QUEUE_PROPERTIES)
	ERROR_CODE(CL_INVALID_COMMAND_QUEUE)
	ERROR_CODE(CL_INVALID_HOST_PTR)
	ERROR_CODE(CL_INVALID_MEM_OBJECT)
	ERROR_CODE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
	ERROR_CODE(CL_INVALID_IMAGE_SIZE)
	ERROR_CODE(CL_INVALID_SAMPLER)
	ERROR_CODE(CL_INVALID_BINARY)
	ERROR_CODE(CL_INVALID_BUILD_OPTIONS)
	ERROR_CODE(CL_INVALID_PROGRAM)
	ERROR_CODE(CL_INVALID_PROGRAM_EXECUTABLE)
	ERROR_CODE(CL_INVALID_KERNEL_NAME)
	ERROR_CODE(CL_INVALID_KERNEL_DEFINITION)
	ERROR_CODE(CL_INVALID_KERNEL)
	ERROR_CODE(CL_INVALID_ARG_INDEX)
	ERROR_CODE(CL_INVALID_ARG_VALUE)
	ERROR_CODE(CL_INVALID_ARG_SIZE)
	ERROR_CODE(CL_INVALID_KERNEL_ARGS)
	ERROR_CODE(CL_INVALID_WORK_DIMENSION)
	ERROR_CODE(CL_INVALID_WORK_GROUP_SIZE)
	ERROR_CODE(CL_INVALID_WORK_ITEM_SIZE)
	ERROR_CODE(CL_INVALID_GLOBAL_OFFSET)
	ERROR_CODE(CL_INVALID_EVENT_WAIT_LIST)
	ERROR_CODE(CL_INVALID_EVENT)
	ERROR_CODE(CL_INVALID_OPERATION)
	ERROR_CODE(CL_INVALID_GL_OBJECT)
	ERROR_CODE(CL_INVALID_BUFFER_SIZE)
	ERROR_CODE(CL_INVALID_MIP_LEVEL)

	// cl_khr_icd
	ERROR_CODE(CL_PLATFORM_NOT_FOUND_KHR)
	}

	static thread_local char buffer[32];
	const int size = std::snprintf(buffer, sizeof(buffer), "Unknown CL error (%d)", error);
	return std::string_view(buffer, size);

#undef ERROR_CODE
}


namespace detail {

void * enqueueMapBuffer(const cl_command_queue queue, const cl_mem buffer, const bool blocking,
		const cl_map_flags flags, const int64_t offset, const int64_t size,
		const int numEventsInWaitList, const cl_event * const eventWaitList,
		ocl::Event * const event)
{
	cl_event clevent;
	cl_int err;
	void * const mem = clEnqueueMapBuffer(queue, buffer, blocking,
			flags, offset, size,
			numEventsInWaitList, eventWaitList,
			event ? &clevent : nullptr,
			&err);
	CHECKCL(err, "clEnqueueMapBuffer() failed")

	if (event) {
		*event = ocl::Event::adapt({}, clevent);
	}

	return mem;
}


void * enqueueMapImage(cl_command_queue queue, cl_mem image, bool blocking,
		cl_map_flags flags, const size_t * origin, const size_t * region,
		size_t & imageRowPitch, size_t & imageSlicePitch,
		int numEventsInWaitList, const cl_event * eventWaitList,
		ocl::Event * event)
{
	cl_event clevent;
	cl_int err;
	void * const mem = clEnqueueMapImage(queue, image, blocking,
			flags, origin, region, &imageRowPitch, &imageSlicePitch,
			numEventsInWaitList, eventWaitList,
			event ? &clevent : nullptr,
			&err);
	CHECKCL(err, "clEnqueueMapBuffer() failed")

	if (event) {
		*event = ocl::Event::adapt({}, clevent);
	}

	return mem;
}


void enqueueUnmapMemObject(const cl_command_queue queue, const cl_mem mem,
		void * const data) noexcept
{
	CHECKCL_NOTHROW(clEnqueueUnmapMemObject(queue, mem, data, 0, nullptr, nullptr))
}

} // detail


void printProfilingInfo(cl_event event)
{
	cl_ulong queuedTime = 0;
	CHECKCL(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_QUEUED,
			sizeof(cl_ulong), &queuedTime, nullptr))

	cl_ulong submittedTime = 0;
	CHECKCL(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_SUBMIT,
			sizeof(cl_ulong), &submittedTime, nullptr))

	cl_ulong startTime = 0;
	CHECKCL(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,
			sizeof(cl_ulong), &startTime, nullptr))

	cl_ulong endTime = 0;
	CHECKCL(clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,
			sizeof(cl_ulong), &endTime, nullptr))

	INFO("Profiling information:")
	/* OpenCL returns times in nano seconds. Print out the times in milliseconds (divide by a million). */
	INFO("Queued time: \t", (submittedTime - queuedTime) / 1000000.0, "ms")
	INFO("Wait time: \t", (startTime - submittedTime) / 1000000.0, "ms")
	INFO("Run time: \t", (endTime - startTime) / 1000000.0, "ms")
}


void printSupported2DImageFormats(cl_context context)
{
	/* Get the number of supported image formats in order to allocate the correct amount of memory. */
	/*cl_uint numberOfImageFormats;
	if (!checkSuccess(clGetSupportedImageFormats(context, 0, CL_MEM_OBJECT_IMAGE3D, 0, NULL, &numberOfImageFormats)))
	{
		cerr << "Getting the number of supported 3D image formats failed. " << __FILE__ << ":"<< __LINE__ << endl;
		return false;
	}*/

	/* Query the first device of the context (the context may have several). */
	cl_device_id device = nullptr;
	CHECKCL(clGetContextInfo(context, CL_CONTEXT_DEVICES,
			sizeof(cl_device_id), &device, nullptr))

	// Get number of supported 2D image formats
	cl_uint numberOfImageFormats = 0;
	CHECKCL(clGetSupportedImageFormats(context, CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D,
			0, nullptr, &numberOfImageFormats))

	/*
	 * CL_DEVICE_IMAGE3D_MAX_WIDTH returns a size_t, so it must be read into a size_t-sized buffer.
	 * A value of 0 indicates that the device does not support 3D images.
	 */
	size_t image3DMaxWidth = 0;
	CHECKCL(clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_WIDTH,
			sizeof(size_t), &image3DMaxWidth, nullptr))
	if (image3DMaxWidth == 0) {
		ERROR("Device does not support 3D images.")
	}

	/* Get the list of supported image formats. */
	std::vector<cl_image_format> imageFormats(numberOfImageFormats);
	CHECKCL(clGetSupportedImageFormats(context, CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D,
			numberOfImageFormats, imageFormats.data(), nullptr))

	INFO(numberOfImageFormats, "Image formats supported",
			numberOfImageFormats > 0 ? "(channel order, channel data type):" : ".")

	for (unsigned int i = 0; i < numberOfImageFormats; i++) {
		INFO(imageChannelOrderToString(imageFormats[i].image_channel_order), ",",
				imageChannelDataTypeToString(imageFormats[i].image_channel_data_type))
	}
}


std::string imageChannelOrderToString(cl_channel_order channelOrder)
{
	switch (channelOrder)
	{
	case CL_R:
		return "CL_R";
	case CL_A:
		return "CL_A";
	case CL_RG:
		return "CL_RG";
	case CL_RA:
		return "CL_RA";
	case CL_RGB:
		return "CL_RGB";
	case CL_RGBA:
		return "CL_RGBA";
	case CL_BGRA:
		return "CL_BGRA";
	case CL_ARGB:
		return "CL_ARGB";
	case CL_INTENSITY:
		return "CL_INTENSITY";
	case CL_LUMINANCE:
		return "CL_LUMINANCE";
	case CL_Rx:
		return "CL_Rx";
	case CL_RGx:
		return "CL_RGx";
	case CL_RGBx:
		return "CL_RGBx";
	default:
		return "Unknown image channel order";
	}
}


std::string imageChannelDataTypeToString(cl_channel_type channelDataType)
{
	switch (channelDataType)
	{
	case CL_SNORM_INT8:
		return "CL_SNORM_INT8";
	case CL_SNORM_INT16:
		return "CL_SNORM_INT16";
	case CL_UNORM_INT8:
		return "CL_UNORM_INT8";
	case CL_UNORM_INT16:
		return "CL_UNORM_INT16";
	case CL_UNORM_SHORT_565:
		return "CL_UNORM_SHORT_565";
	case CL_UNORM_SHORT_555:
		return "CL_UNORM_SHORT_555";
	case CL_UNORM_INT_101010:
		return "CL_UNORM_INT_101010";
	case CL_SIGNED_INT8:
		return "CL_SIGNED_INT8";
	case CL_SIGNED_INT16:
		return "CL_SIGNED_INT16";
	case CL_SIGNED_INT32:
		return "CL_SIGNED_INT32";
	case CL_UNSIGNED_INT8:
		return "CL_UNSIGNED_INT8";
	case CL_UNSIGNED_INT16:
		return "CL_UNSIGNED_INT16";
	case CL_UNSIGNED_INT32:
		return "CL_UNSIGNED_INT32";
	case CL_HALF_FLOAT:
		return "CL_HALF_FLOAT";
	case CL_FLOAT:
		return "CL_FLOAT";
	default:
		return "Unknown image channel data type";
	}
}


bool isExtensionSupported(cl_device_id device, std::string extension)
{
	if (extension.empty())
	{
		return false;
	}

	/* First find out how large the ouput of the OpenCL device query will be. */
	size_t extensionsReturnSize = 0;
	CHECKCL(clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, nullptr, &extensionsReturnSize))

	/* Allocate enough memory for the output. */
	std::vector<char> extensions(extensionsReturnSize);

	/* Get the list of all extensions supported. */
	CHECKCL(clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, extensionsReturnSize, extensions.data(), nullptr))

	/* See if the requested extension is in the list. */
	std::string extensionsString(extensions.data());
	bool returnResult = false;
	if (extensionsString.find(extension) != std::string::npos)
	{
		returnResult = true;
	}

	return returnResult;
}


} // ocl




namespace HandleWrapper {
namespace detail {




ocl::Context Handle<cl_context>::create()
{
	cl_uint platformCount = 0;
	cl_platform_id first = 0;
	CHECKCL(clGetPlatformIDs(1, &first, &platformCount))

	CHECK(platformCount > 0, "No OpenCL platforms found")

	const cl_context_properties props[] = {
		CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(first),
		0
	};

	cl_int error = CL_SUCCESS;
	const cl_context context = clCreateContextFromType(props, CL_DEVICE_TYPE_ALL,
			nullptr, nullptr, &error);
	CHECKCL(error, "clCreateContextFromType() failed")

	return ocl::Context::adapt({}, context);
}


void Handle<cl_context>::destroy(const Data & data, const cl_context & context) noexcept
{
	CHECKCL_NOTHROW(clReleaseContext(context))
}


cl_device_id Handle<cl_context>::findDevice() const
{
	const ocl::Context & context = cast();

	size_t deviceBufferSize = -1;
	CHECKCL(clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, nullptr, &deviceBufferSize))
	CHECK(deviceBufferSize != 0)

	std::vector<cl_device_id> devices(deviceBufferSize / sizeof(cl_device_id));
	CHECKCL(clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices.data(), nullptr))

	return devices[0];
}


ocl::CommandQueue Handle<cl_context>::createCommandQueue(
		const cl_device_id device) const
{
	const ocl::Context & context = cast();

	const cl_command_queue_properties props[] = {
		CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE,
		0
	};

	const auto queue = [&context, device, &props] () -> cl_command_queue {
		cl_int err;
#if CL_TARGET_OPENCL_VERSION >= 300
		const cl_command_queue queue = clCreateCommandQueueWithProperties(
				context, device, props, &err);
#else
		const cl_command_queue queue = clCreateCommandQueue(
				context, device, CL_QUEUE_PROFILING_ENABLE, &err);
#endif
		CHECKCL(err)
		return queue;
	}();

	return ocl::CommandQueue::adapt({}, queue);
}


ocl::Program Handle<cl_context>::createProgram(const cl_device_id device,
		const std::filesystem::path & kernelFilePath) const
{
	const ocl::Context & context = cast();

	const std::string code = readTextFile(kernelFilePath);

	cl_int err;
	const char * codep = code.c_str();
	const cl_program program = clCreateProgramWithSource(context, 1, &codep, nullptr, &err);
	CHECKCL(err)
	CHECK(program)

	try {
		CHECKCL(clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr))
	} catch (const Exception & e) {
		size_t logSize = 0;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);

		std::vector<char> logBuffer;

		const auto log = [&] () -> std::string_view {
			if (logSize > 1) {
				logBuffer.resize(logSize);
				clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize,
						logBuffer.data(), nullptr);
				return std::string_view(logBuffer.data(), logSize - 1);
			} else {
				return {};
			}
		}();

		clReleaseProgram(program);

		THROW("Bulid error:", log)
	}

	return ocl::Program::adapt({}, program);
}


ocl::Mem Handle<cl_context>::createBuffer(const cl_mem_flags flags, const int64_t size) const
{
	cl_int err;
	const cl_mem mem = clCreateBuffer(cast(), flags, size, nullptr, &err);
	CHECKCL(err, "clCreateBuffer() failed")
	CHECK(mem)

	return ocl::Mem::adapt({}, mem);
}


ocl::Mem Handle<cl_context>::createImage(const cl_mem_flags flags,
		const cl_image_format & format, const cl_image_desc & desc,
		void * const hostPtr) const
{
	cl_int err;
	const cl_mem mem = clCreateImage(cast(), flags, &format, &desc, hostPtr, &err);
	CHECKCL(err, "clCreateImage() failed")
	CHECK(mem)

	return ocl::Mem::adapt({}, mem);
}




void Handle<cl_event>::destroy(const Data & data, const cl_event & event) noexcept
{
	CHECKCL_NOTHROW(clReleaseEvent(event))
}




void Handle<cl_command_queue>::destroy(const Data & data,
		const cl_command_queue & queue) noexcept
{
	CHECKCL_NOTHROW(clReleaseCommandQueue(queue))
}


ocl::Event Handle<cl_command_queue>::enqueueNDRangeKernel(const cl_kernel kernel,
		const cl_uint workDim,
		const size_t * const globalWorkOffset,
		const size_t * const globalWorkSize,
		const size_t * const localWorkSize,
		const cl_uint numEentsInWaitList,
		const cl_event * const eventWaitList,
		const bool createEvent) const
{
	cl_event event;
	CHECKCL(clEnqueueNDRangeKernel(cast(), kernel,
			workDim, globalWorkOffset, globalWorkSize, localWorkSize,
			numEentsInWaitList, eventWaitList,
			createEvent ? &event : nullptr))

	if (createEvent) {
		return ocl::Event::adapt({}, event);
	} else {
		return ocl::Event::null();
	}
}




void Handle<cl_program>::destroy(const Data & data, const cl_program & program) noexcept
{
	CHECKCL_NOTHROW(clReleaseProgram(program))
}


ocl::Kernel Handle<cl_program>::createKernel(const char * name) const
{
	const ocl::Program & program = cast();

	cl_int err;
	const cl_kernel kernel = clCreateKernel(program, name, &err);
	CHECKCL(err)
	CHECK(kernel)

	return ocl::Kernel::adapt({}, kernel);
}




void Handle<cl_kernel>::destroy(const Data & data, const cl_kernel & kernel) noexcept
{
	CHECKCL_NOTHROW(clReleaseKernel(kernel))
}


void Handle<cl_kernel>::_setArg(const int index, const size_t size, const void * const value) const
{
	CHECKCL(clSetKernelArg(cast(), index, size, value))
}




void Handle<cl_mem>::destroy(const Data & data, const cl_mem & mem) noexcept
{
	CHECKCL_NOTHROW(clReleaseMemObject(mem))
}




}} // HandleWrapper::detail
