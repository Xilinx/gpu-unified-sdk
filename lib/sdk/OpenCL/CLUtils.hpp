
#pragma once

#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 300
#endif

#include <CL/cl.h>

#include <filesystem>
#include <string>
#include <string_view>

#include <Common/HandleWrapper.hpp>
#include <Common/Log.hpp>




namespace HandleWrapper::detail {

template <typename T>
struct CLTraits {
	inline static constexpr T NullValue = nullptr;
};

template <> struct Traits<cl_context> : CLTraits<cl_context> {};
template <> struct Traits<cl_event> : CLTraits<cl_event> {};
template <> struct Traits<cl_command_queue> : CLTraits<cl_command_queue> {};
template <> struct Traits<cl_program> : CLTraits<cl_program> {};
template <> struct Traits<cl_kernel> : CLTraits<cl_kernel> {};
template <> struct Traits<cl_mem> : CLTraits<cl_mem> {};

template <typename T> struct EnqueueBufferRet;
template <typename T> struct EnqueueImageRet;

}




namespace ocl {


using Context      = HandleWrapper::detail::Wrapper<cl_context>;
using Event        = HandleWrapper::detail::Wrapper<cl_event>;
using CommandQueue = HandleWrapper::detail::Wrapper<cl_command_queue>;
using Program      = HandleWrapper::detail::Wrapper<cl_program>;
using Kernel       = HandleWrapper::detail::Wrapper<cl_kernel>;
using Mem          = HandleWrapper::detail::Wrapper<cl_mem>;


namespace detail {
	extern void * enqueueMapBuffer(cl_command_queue queue, cl_mem buffer, bool blocking,
			cl_map_flags flags, int64_t offset, int64_t size,
			int numEventsInWaitList, const cl_event * eventWaitList,
			ocl::Event * event);
	extern void * enqueueMapImage(cl_command_queue queue, cl_mem image, bool blocking,
			cl_map_flags flags, const size_t * origin, const size_t * region,
			size_t & imageRowPitch, size_t & imageSlicePitch,
			int numEventsInWaitList, const cl_event * eventWaitList,
			ocl::Event * event);
	extern void enqueueUnmapMemObject(cl_command_queue queue, cl_mem mem, void * data) noexcept;
}


extern std::string_view errorCodeToString(cl_int error) noexcept;
extern void printProfilingInfo(cl_event event);
extern void printSupported2DImageFormats(cl_context context);
extern std::string imageChannelOrderToString(cl_channel_order channelOrder);
extern std::string imageChannelDataTypeToString(cl_channel_type channelDataType);
extern bool isExtensionSupported(cl_device_id device, std::string extension);



template <typename T>
class MappedMemory {
public:
	MappedMemory(cl_command_queue queue, cl_mem mem, void * data) noexcept :
		queue_(queue), mem_(mem), data_(reinterpret_cast<T*>(data))
	{}

	MappedMemory(MappedMemory && other) noexcept :
		queue_(other.queue_),
		mem_(other.mem_),
		data_(std::exchange(other.data_, nullptr))
	{}

	MappedMemory & operator=(MappedMemory && other) noexcept
	{
		if (data_) {
			ocl::detail::enqueueUnmapMemObject(queue_, mem_, data_);
		}
		queue_ = other.queue_;
		mem_ = other.mem_;
		data_ = std::exchange(other.data_, nullptr);
		return *this;
	}

	MappedMemory() = default;

	MappedMemory(const MappedMemory &) = delete;
	MappedMemory & operator=(const MappedMemory &) = delete;

	~MappedMemory() noexcept {
		if (data_) {
			ocl::detail::enqueueUnmapMemObject(queue_, mem_, data_);
		}
	}

	T * data() const noexcept { return data_; }

private:
	cl_command_queue queue_;
	cl_mem mem_;
	T * data_ = nullptr;
};




template <typename T>
using EnqueueBufferRet = HandleWrapper::detail::EnqueueBufferRet<T>;

template <typename T>
using EnqueueImageRet = HandleWrapper::detail::EnqueueImageRet<T>;




} // ocl




#define CHECKCL(cond, ...) \
	if (const cl_int error = cond; error != CL_SUCCESS) [[unlikely]] { \
		throw (ExceptionMaker(Log::context(Log::Level::Error, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), \
				"CL error:", ocl::errorCodeToString(error), Log::format("code: %d", error), \
				##__VA_ARGS__).make(); \
	}

#define CHECKCL_NOTHROW(cond, ...) \
	if (const cl_int error = cond; error != CL_SUCCESS) [[unlikely]] { \
		(Log(Log::context(Log::Level::Error, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), \
				"CL error:", ocl::errorCodeToString(error), Log::braced(error), Log::quoted(#cond), \
				##__VA_ARGS__).print(Log::Stream::Type::Error); \
	}




namespace HandleWrapper {
namespace detail {


template <>
class Handle<cl_context> : public Base<cl_context> {
public:
	struct Data {};

	static ocl::Context create();
	static void destroy(const Data & data, const cl_context & context) noexcept;

	cl_device_id findDevice() const;
	ocl::CommandQueue createCommandQueue(cl_device_id device) const;
	ocl::Program createProgram(cl_device_id device,
			const std::filesystem::path & kernelFilePath) const;
	ocl::Mem createBuffer(cl_mem_flags flags, int64_t size) const;
	ocl::Mem createImage(cl_mem_flags flags, const cl_image_format & format,
			const cl_image_desc & desc, void * hostPtr) const;
};


template <>
class Handle<cl_event> : public Base<cl_event> {
public:
	struct Data {};

	static void destroy(const Data & data, const cl_event & event) noexcept;
};


template <typename T>
struct EnqueueBufferRet {
	ocl::MappedMemory<T> memory;
	ocl::Event event;
};


template <typename T>
struct EnqueueImageRet {
	ocl::MappedMemory<T> memory;
	ocl::Event event;
	size_t rowPitch;
	size_t slicePitch;
};


template <>
class Handle<cl_command_queue> : public Base<cl_command_queue> {
public:
	struct Data {};

	static void destroy(const Data & data, const cl_command_queue & queue) noexcept;

	template <typename T>
	ocl::EnqueueBufferRet<T> enqueueMapBuffer(cl_mem buffer, bool blocking, cl_map_flags flags,
			int64_t offset, int64_t size, bool createEvent) const
	{
		const ocl::CommandQueue & queue = cast();
		ocl::EnqueueBufferRet<T> ret{
			.event = ocl::Event::null(),
		};
		void * const data = ocl::detail::enqueueMapBuffer(
				queue, buffer, blocking, flags, offset, size,
				0, nullptr,
				createEvent ? &ret.event : nullptr);
		ret.memory = ocl::MappedMemory<T>(queue, buffer, data);
		return ret;
	}

	template <typename T>
	ocl::EnqueueImageRet<T> enqueueMapImage(cl_mem image, bool blocking, cl_map_flags flags,
			const size_t * origin, const size_t * region,
			bool createEvent) const
	{
		const ocl::CommandQueue & queue = cast();
		ocl::EnqueueImageRet<T> ret{
			.event = ocl::Event::null(),
		};
		void * const data = ocl::detail::enqueueMapImage(
				queue, image, blocking, flags, origin, region,
				ret.rowPitch, ret.slicePitch,
				0, nullptr,
				createEvent ? &ret.event : nullptr);
		ret.memory = ocl::MappedMemory<T>(queue, image, data);
		return ret;
	}

	ocl::Event enqueueNDRangeKernel(cl_kernel kernel,
			cl_uint workDim,
			const size_t * globalWorkOffset,
			const size_t * globalWorkSize,
			const size_t * localWorkSize,
			cl_uint numEentsInWaitList,
			const cl_event * eventWaitList,
			bool createEvent) const;
};




template <>
class Handle<cl_program> : public Base<cl_program> {
public:
	struct Data {};

	static void destroy(const Data & data, const cl_program & program) noexcept;

	ocl::Kernel createKernel(const char * name) const;
};




template <>
class Handle<cl_kernel> : public Base<cl_kernel> {
public:
	struct Data {};

	static void destroy(const Data & data, const cl_kernel & kernel) noexcept;

	template <typename CLT, typename T>
	void setArg(int index, const T & value) const {
		const CLT cvalue = static_cast<CLT>(value);
		_setArg(index, sizeof(CLT), &cvalue);
	}

private:
	void _setArg(int index, size_t size, const void * value) const;
};




template <>
class Handle<cl_mem> : public Base<cl_mem> {
public:
	struct Data {};

	static void destroy(const Data & data, const cl_mem & mem) noexcept;
};




}} // HandleWrapper::detail
