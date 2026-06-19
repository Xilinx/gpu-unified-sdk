
#pragma once

#include <string_view>
#include <vector>

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "Common/Log.hpp"





#define CHECKVK(x, ...) \
	if (const VkResult res = x; res != VK_SUCCESS) [[unlikely]] { \
		throw (ExceptionMaker(Log::context(Log::Level::Error, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), \
				"Vulkan error:", Log::quoted(#x), Log::format("code: %0x08x", res), \
				##__VA_ARGS__).make(); \
	}




namespace vk {




class Extensions {
public:
	struct Item {
		std::string_view name;
		int specVersion;
	};

	static Extensions fromProperties(std::vector<VkExtensionProperties> && properties) noexcept
	{ return Extensions(std::move(properties)); }

	static Extensions fromList(const std::vector<Item> & items) noexcept;

	static Extensions instanceExtensions(const char * layerName);
	static Extensions gpuExtensions(VkPhysicalDevice gpu, const char * layerName);

	Extensions(Extensions && other) noexcept :
		properties_(std::move(other.properties_)),
		list_(_makeList(properties_))
	{}

	const std::vector<Item> & list() const noexcept { return list_; }

	bool contains(const std::string_view & name, int minSpecVersion) const noexcept;

private:
	static std::vector<Item> _makeList(const std::vector<VkExtensionProperties> & properties) noexcept;

	Extensions(std::vector<VkExtensionProperties> && properties) noexcept :
		properties_(std::move(properties)),
		list_(_makeList(properties_))
	{}

	std::vector<VkExtensionProperties> properties_;
	std::vector<Item> list_;
};

struct LayerInfo {
	VkLayerProperties properties;
	Extensions extensions;
};




class Version {
public:
	Version(uint32_t version) noexcept :
		version_(version)
	{}

	int major() const noexcept { return VK_API_VERSION_MAJOR(version_); }
	int minor() const noexcept { return VK_API_VERSION_MINOR(version_); }
	int patch() const noexcept { return VK_API_VERSION_PATCH(version_); }

	std::string str() const noexcept
	{
		std::ostringstream s;
		s << major() << '.' << minor() << '.' << patch();
		return s.str();
	}

private:
	const uint32_t version_;
};




namespace detail {

template <typename T>
struct NullTraits {
	static T null() noexcept { return VK_NULL_HANDLE; }
	static bool isNull(T handle) noexcept { return handle == null(); }
};

template <typename T>
struct AllocTraits;

template <typename T>
class Handle : public AllocTraits<T> {
public:
	using Data = AllocTraits<T>::Data;

	static Handle<T> null() noexcept {
		return Handle<T>(Data{}, NullTraits<T>::null());
	}

	static Handle<T> adapt(const Data & data, const T & handle) noexcept
	{
		return Handle<T>(data, handle);
	}

	~Handle() noexcept
	{
		if (!NullTraits<T>::isNull(handle_)) {
			AllocTraits<T>::destroy(data_, handle_);
		}
	}

	const Data & data() const noexcept { return data_; }
	const T & handle() const noexcept { return handle_; }
	operator const T &() const noexcept { return handle_; }
	operator bool() const noexcept { return !NullTraits<T>::isNull(handle_); }

	Handle(Handle<T> && other) noexcept :
		data_(other.data_),
		handle_(std::exchange(other.handle_, NullTraits<T>::null()))
	{}

	Handle<T> & operator=(Handle<T> && other) noexcept
	{
		if (!NullTraits<T>::isNull(handle_)) {
			AllocTraits<T>::destroy(data_, handle_);
		}
		data_ = other.data_;
		handle_ = std::exchange(other.handle_, NullTraits<T>::null());
		return *this;
	}

	Handle() = delete;
	Handle(const Handle &) = delete;
	Handle & operator=(const Handle &) = delete;

private:
	explicit Handle(const Data & data, const T & handle) noexcept :
		data_(data),
		handle_(handle)
	{}

	Data data_;
	T handle_;
};

} // detail




namespace detail {

template <>
struct AllocTraits<VkDeviceMemory> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkDeviceMemory> create(const VkDevice device, const VkMemoryAllocateInfo & info)
	{
		VkDeviceMemory handle;
		CHECKVK(vkAllocateMemory(device, &info, nullptr, &handle))
		return Handle<VkDeviceMemory>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkDeviceMemory & value) noexcept
	{
		vkFreeMemory(data.device, value, nullptr);
	}
};

} // detail

using DeviceMemory = detail::Handle<VkDeviceMemory>;




namespace detail {

template <>
struct AllocTraits<VkSemaphore> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkSemaphore> create(const VkDevice device,
			const VkSemaphoreCreateInfo & info)
	{
		VkSemaphore handle;
		CHECKVK(vkCreateSemaphore(device, &info, nullptr, &handle))
		return Handle<VkSemaphore>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkSemaphore & handle) noexcept
	{
		vkDestroySemaphore(data.device, handle, nullptr);
	}
};

} // detail

using Semaphore = detail::Handle<VkSemaphore>;




namespace detail {

template <>
struct AllocTraits<VkFence> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkFence> create(const VkDevice device, const VkFenceCreateInfo & info)
	{
		VkFence handle;
		CHECKVK(vkCreateFence(device, &info, nullptr, &handle))
		return Handle<VkFence>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkFence & value) noexcept
	{
		vkDestroyFence(data.device, value, nullptr);
	}
};

} // detail

using Fence = detail::Handle<VkFence>;




namespace detail {

template <>
struct AllocTraits<VkInstance> {
	struct Data {};

	static Handle<VkInstance> create(const VkInstanceCreateInfo & info)
	{
		VkInstance handle;
		CHECKVK(vkCreateInstance(&info, nullptr, &handle))
		return Handle<VkInstance>::adapt({}, handle);
	}

	static void destroy(const Data &, const VkInstance & handle) noexcept
	{
		::vkDestroyInstance(handle, nullptr);
	}
};

} // detail

using Instance = detail::Handle<VkInstance>;




namespace detail {

template <>
struct AllocTraits<VkDevice> {
	struct Data {
		VkPhysicalDevice gpu;
	};

	static Handle<VkDevice> create(const VkPhysicalDevice gpu, const VkDeviceCreateInfo & info)
	{
		VkDevice handle;
		CHECKVK(vkCreateDevice(gpu, &info, nullptr, &handle))
		return Handle<VkDevice>::adapt({gpu}, handle);
	}

	static void destroy(const Data &, const VkDevice & handle) noexcept
	{
		vkDestroyDevice(handle, nullptr);
	}
};

} // detail

using Device = detail::Handle<VkDevice>;




namespace detail {

template <>
struct AllocTraits<VkCommandPool> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkCommandPool> create(const VkDevice device,
			const VkCommandPoolCreateInfo & info)
	{
		VkCommandPool handle;
		CHECKVK(vkCreateCommandPool(device, &info, nullptr, &handle))
		return Handle<VkCommandPool>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkCommandPool & handle) noexcept
	{
		vkDestroyCommandPool(data.device, handle, nullptr);
	}
};

} // detail

using CommandPool = detail::Handle<VkCommandPool> ;




namespace detail {

template <>
struct AllocTraits<VkRenderPass> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkRenderPass> create(const VkDevice device,
			const VkRenderPassCreateInfo & info)
	{
		VkRenderPass handle;
		CHECKVK(vkCreateRenderPass(device, &info, nullptr, &handle))
		return Handle<VkRenderPass>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkRenderPass & handle) noexcept
	{
		vkDestroyRenderPass(data.device, handle, nullptr);
	}
};

} // detail

using RenderPass = detail::Handle<VkRenderPass>;




namespace detail {

template <>
struct AllocTraits<VkPipelineLayout> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkPipelineLayout> create(const VkDevice device,
			const VkPipelineLayoutCreateInfo & info)
	{
		VkPipelineLayout handle;
		CHECKVK(vkCreatePipelineLayout(device, &info, nullptr, &handle))
		return Handle<VkPipelineLayout>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkPipelineLayout & handle) noexcept
	{
		vkDestroyPipelineLayout(data.device, handle, nullptr);
	}
};

} // detail

using PipelineLayout = detail::Handle<VkPipelineLayout>;




namespace detail {

template <>
struct AllocTraits<VkPipelineCache> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkPipelineCache> create(const VkDevice device,
			const VkPipelineCacheCreateInfo & info)
	{
		VkPipelineCache handle;
		CHECKVK(vkCreatePipelineCache(device, &info, nullptr, &handle))
		return Handle<VkPipelineCache>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkPipelineCache & handle) noexcept
	{
		vkDestroyPipelineCache(data.device, handle, nullptr);
	}
};

} // detail

using PipelineCache = detail::Handle<VkPipelineCache>;




namespace detail {

template <>
struct AllocTraits<VkPipeline> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkPipeline> create(const VkDevice device, const VkPipelineCache pipelineCache,
			const VkGraphicsPipelineCreateInfo & info)
	{
		VkPipeline handle;
		CHECKVK(vkCreateGraphicsPipelines(device, pipelineCache, 1, &info, nullptr, &handle))
		return Handle<VkPipeline>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkPipeline & handle) noexcept
	{
		vkDestroyPipeline(data.device, handle, nullptr);
	}
};

} // detail

using Pipeline = detail::Handle<VkPipeline>;




namespace detail {

template <>
struct AllocTraits<VkImage> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkImage> create(const VkDevice device, const VkImageCreateInfo & info)
	{
		VkImage handle;
		CHECKVK(vkCreateImage(device, &info, nullptr, &handle))
		return Handle<VkImage>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkImage & handle) noexcept
	{
		vkDestroyImage(data.device, handle, nullptr);
	}
};

} // detail

using Image = detail::Handle<VkImage>;




namespace detail {

template <>
struct AllocTraits<VkImageView> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkImageView> create(const VkDevice device,
			const VkImageViewCreateInfo & info)
	{
		VkImageView handle;
		CHECKVK(vkCreateImageView(device, &info, nullptr, &handle))
		return Handle<VkImageView>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkImageView & handle) noexcept
	{
		vkDestroyImageView(data.device, handle, nullptr);
	}
};

} // detail

using ImageView = detail::Handle<VkImageView>;




namespace detail {

template <>
struct AllocTraits<VkFramebuffer> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkFramebuffer> create(const VkDevice device, const VkFramebufferCreateInfo & info)
	{
		VkFramebuffer handle;
		CHECKVK(vkCreateFramebuffer(device, &info, nullptr, &handle))
		return Handle<VkFramebuffer>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkFramebuffer & handle) noexcept
	{
		vkDestroyFramebuffer(data.device, handle, nullptr);
	}
};

} // detail

using Framebuffer = detail::Handle<VkFramebuffer>;




namespace detail {

template <>
struct AllocTraits<VkCommandBuffer> {
	struct Data {
		VkDevice device;
		VkCommandPool commandPool;
	};

	static Handle<VkCommandBuffer> create(const VkDevice device, const VkCommandPool commandPool,
			const VkCommandBufferAllocateInfo & info)
	{
		ASSERT(info.commandBufferCount == 1)
		VkCommandBuffer handle;
		CHECKVK(vkAllocateCommandBuffers(device, &info, &handle))
		return Handle<VkCommandBuffer>::adapt({device, commandPool}, handle);
	}

	static void destroy(const Data & data, const VkCommandBuffer & handle) noexcept
	{
		vkFreeCommandBuffers(data.device, data.commandPool, 1, &handle);
	}
};

} // detail

using CommandBuffer = detail::Handle<VkCommandBuffer>;




namespace detail {

template <>
struct AllocTraits<VkSurfaceKHR> {
	struct Data {
		VkInstance instance;
	};

	static void destroy(const Data & data, const VkSurfaceKHR & handle) noexcept
	{
		vkDestroySurfaceKHR(data.instance, handle, nullptr);
	}
};

} // detail

using Surface = detail::Handle<VkSurfaceKHR>;




namespace detail {

template <>
struct AllocTraits<VkSwapchainKHR> {
	struct Data {
		VkDevice device;
	};

	static Handle<VkSwapchainKHR> create(const VkDevice device,
			const VkSwapchainCreateInfoKHR & info)
	{
		VkSwapchainKHR handle;
		CHECKVK(vkCreateSwapchainKHR(device, &info, nullptr, &handle))
		return Handle<VkSwapchainKHR>::adapt({device}, handle);
	}

	static void destroy(const Data & data, const VkSwapchainKHR & value) noexcept
	{
		vkDestroySwapchainKHR(data.device, value, nullptr);
	}
};

} //detail

using Swapchain = detail::Handle<VkSwapchainKHR>;




extern std::vector<VkPhysicalDevice> enumerateGpus(VkInstance instance);
extern std::vector<VkQueueFamilyProperties> getGpuQueueFamilyProperties(VkPhysicalDevice gpu) noexcept;
extern std::vector<VkPresentModeKHR> getGpuSurfacePresentModes(VkPhysicalDevice gpu, VkSurfaceKHR surface);




} // vk




template <>
struct Log::Write<vk::Version> {
	static void write(std::ostream & s, const vk::Version & v) noexcept
	{
		s << Log::format("%d.%d.%d", v.major(), v.minor(), v.patch());
	}
};
