/*
 * Ported from ssw-mktg/GPU_SDK_Vulkan/platform/wsi/wsi.cpp.
 *
 * Deviations from the original, all required by the unified SDK environment:
 *  - No vulkanSymbolWrapper / libvulkan-stub manual symbol loading. We link the
 *    Vulkan loader directly (Vulkan::Vulkan), so all vk* entry points including
 *    KHR surface/swapchain extensions are called directly.
 *  - loadInstanceSymbols()/loadDeviceSymbols() are therefore removed.
 *  - Validation layers / debug report are not enabled.
 * The swapchain creation, queue selection, acquire/present and the
 * semaphore-based synchronisation are identical to the original.
 */

#include "wsi.hpp"
#include <string.h>

using namespace std;

namespace VersalGPUSDK
{

WSIPlatform::~WSIPlatform()
{
	terminate();
}

bool WSIPlatform::validateExtensions(const vector<const char *> &required,
                                     const std::vector<VkExtensionProperties> &available)
{
	for (auto extension : required)
	{
		bool found = false;
		for (auto &availableExtension : available)
		{
			if (strcmp(availableExtension.extensionName, extension) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
			return false;
	}

	return true;
}

Result WSIPlatform::initialize()
{
	pContext = new Context();
	if (!pContext)
		return RESULT_ERROR_OUT_OF_MEMORY;

	return RESULT_SUCCESS;
}

Result WSIPlatform::initVulkan(const SwapchainDimensions &swapchain,
                               const vector<const char *> &requiredInstanceExtensions,
                               const vector<const char *> &requiredDeviceExtensions)
{
	uint32_t instanceExtensionCount;
	vector<const char *> activeInstanceExtensions;

	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr));
	vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data()));

	for (auto &instanceExt : instanceExtensions)
		LOGI("Instance extension: %s\n", instanceExt.extensionName);

	bool useInstanceExtensions = true;
	if (!validateExtensions(requiredInstanceExtensions, instanceExtensions))
	{
		LOGI("Required instance extensions are missing, will try without.\n");
		useInstanceExtensions = false;
	}
	else
		activeInstanceExtensions = requiredInstanceExtensions;

	VkApplicationInfo app = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	app.pApplicationName = "GPU SDK - AMD Versal";
	app.applicationVersion = 0;
	app.pEngineName = "GPU SDK - AMD Versal";
	app.engineVersion = 0;
	app.apiVersion = VK_MAKE_VERSION(1, 0, 24);

	VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instanceInfo.pApplicationInfo = &app;
	if (useInstanceExtensions)
	{
		instanceInfo.enabledExtensionCount = activeInstanceExtensions.size();
		instanceInfo.ppEnabledExtensionNames = activeInstanceExtensions.data();
	}

	// Create the Vulkan instance
	{
		VkResult res = vkCreateInstance(&instanceInfo, nullptr, &instance);

		// Try to fall back to compatible Vulkan versions if the driver is using
		// older, but compatible API versions.
		if (res == VK_ERROR_INCOMPATIBLE_DRIVER)
		{
			app.apiVersion = VK_MAKE_VERSION(1, 0, 1);
			res = vkCreateInstance(&instanceInfo, nullptr, &instance);
			if (res == VK_SUCCESS)
				LOGI("Created Vulkan instance with API version 1.0.1.\n");
		}

		if (res == VK_ERROR_INCOMPATIBLE_DRIVER)
		{
			app.apiVersion = VK_MAKE_VERSION(1, 0, 2);
			res = vkCreateInstance(&instanceInfo, nullptr, &instance);
			if (res == VK_SUCCESS)
				LOGI("Created Vulkan instance with API version 1.0.2.\n");
		}

		if (res != VK_SUCCESS)
		{
			LOGE("Failed to create Vulkan instance (error: %d).\n", int(res));
			return RESULT_ERROR_GENERIC;
		}
	}

	uint32_t gpuCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));

	if (gpuCount < 1)
	{
		LOGE("Failed to enumerate Vulkan physical device.\n");
		return RESULT_ERROR_GENERIC;
	}

	vector<VkPhysicalDevice> gpus(gpuCount);
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data()));

	gpu = VK_NULL_HANDLE;
	for (auto device : gpus)
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		// If we have multiple GPUs in our system, try to find a Mali device.
		if (strstr(properties.deviceName, "Mali"))
		{
			gpu = device;
			LOGI("Found ARM Mali physical device: %s.\n", properties.deviceName);
			break;
		}
	}

	// Fallback to the first GPU we find in the system.
	if (gpu == VK_NULL_HANDLE)
		gpu = gpus.front();

	vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
	vkGetPhysicalDeviceMemoryProperties(gpu, &memoryProperties);

	uint32_t queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueCount, nullptr);
	queueProperties.resize(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueCount, queueProperties.data());
	if (queueCount < 1)
	{
		LOGE("Failed to query number of queues.");
		return RESULT_ERROR_GENERIC;
	}

	uint32_t deviceExtensionCount;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu, nullptr, &deviceExtensionCount, nullptr));
	vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu, nullptr, &deviceExtensionCount, deviceExtensions.data()));

	for (auto &deviceExt : deviceExtensions)
		LOGI("Device extension: %s\n", deviceExt.extensionName);

	bool useDeviceExtensions = true;
	if (!validateExtensions(requiredDeviceExtensions, deviceExtensions))
	{
		LOGI("Required device extensions are missing, will try without.\n");
		useDeviceExtensions = false;
	}

	surface = createSurface();
	if (surface == VK_NULL_HANDLE)
	{
		LOGE("Failed to create surface.");
		return RESULT_ERROR_GENERIC;
	}

	bool foundQueue = false;
	for (unsigned i = 0; i < queueCount; i++)
	{
		VkBool32 supportsPresent;

		// There must exist at least one queue that has graphics and compute
		// support.
		const VkQueueFlags required = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

		vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &supportsPresent);

		// We want a queue which supports all of graphics, compute and presentation.
		if (((queueProperties[i].queueFlags & required) == required) && supportsPresent)
		{
			graphicsQueueIndex = i;
			foundQueue = true;
			break;
		}
	}

	if (!foundQueue)
	{
		LOGE("Did not find suitable queue which supports graphics, compute and "
		     "presentation.\n");
		return RESULT_ERROR_GENERIC;
	}

	static const float one = 1.0f;
	VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queueInfo.queueFamilyIndex = graphicsQueueIndex;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = &one;

	VkPhysicalDeviceFeatures features = { false };
	VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	if (useDeviceExtensions)
	{
		deviceInfo.enabledExtensionCount = requiredDeviceExtensions.size();
		deviceInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
	}

	deviceInfo.pEnabledFeatures = &features;

	VK_CHECK(vkCreateDevice(gpu, &deviceInfo, nullptr, &device));

	vkGetDeviceQueue(device, graphicsQueueIndex, 0, &queue);

	Result res = initSwapchain(swapchain);
	if (res != RESULT_SUCCESS)
	{
		LOGE("Failed to init swapchain.");
		return res;
	}

	res = pContext->onPlatformUpdate(this);
	if (FAILED(res))
		return res;

	semaphoreManager = new SemaphoreManager(device);
	return RESULT_SUCCESS;
}

void WSIPlatform::destroySwapchain()
{
	if (device)
		vkDeviceWaitIdle(device);

	if (swapchain)
	{
		vkDestroySwapchainKHR(device, swapchain, nullptr);
		swapchain = VK_NULL_HANDLE;
	}
}

void WSIPlatform::terminate()
{
	// Don't release anything until the GPU is completely idle.
	if (device)
		vkDeviceWaitIdle(device);

	delete semaphoreManager;
	semaphoreManager = nullptr;

	// Make sure we tear down the context before destroying the device since
	// context
	// also owns some Vulkan resources.
	delete pContext;
	pContext = nullptr;

	destroySwapchain();

	if (surface)
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
		surface = VK_NULL_HANDLE;
	}

	if (device)
	{
		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;
	}

	if (instance)
	{
		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;
	}
}

Result WSIPlatform::initSwapchain(const SwapchainDimensions &dim)
{
	VkSurfaceCapabilitiesKHR surfaceProperties;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceProperties));

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);
	vector<VkSurfaceFormatKHR> formats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, formats.data());

	VkSurfaceFormatKHR format;
	if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		format = formats[0];
		format.format = dim.format;
	}
	else
	{
		if (formatCount == 0)
		{
			LOGE("Surface has no formats.\n");
			return RESULT_ERROR_GENERIC;
		}

		format.format = VK_FORMAT_UNDEFINED;
		for (auto &candidate : formats)
		{
			switch (candidate.format)
			{
				// Favor UNORM formats as the samples are not written for sRGB currently.
				case VK_FORMAT_R8G8B8A8_UNORM:
				case VK_FORMAT_B8G8R8A8_UNORM:
				case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
					format = candidate;
					break;

				default:
					break;
			}

			if (format.format != VK_FORMAT_UNDEFINED)
				break;
		}

		if (format.format == VK_FORMAT_UNDEFINED)
			format = formats[0];
	}

	VkExtent2D swapchainSize;
	// -1u is a magic value (in Vulkan specification) which means there's no fixed
	// size.
	if (surfaceProperties.currentExtent.width == -1u)
	{
		swapchainSize.width = dim.width;
		swapchainSize.height = dim.height;
	}
	else
		swapchainSize = surfaceProperties.currentExtent;

	// FIFO must be supported by all implementations.
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	// Determine the number of VkImage's to use in the swapchain.
	// Ideally, we desire to own 1 image at a time, the rest of the images can
	// either be rendered to and/or
	// being queued up for display.
	uint32_t desiredSwapchainImages = surfaceProperties.minImageCount + 1;
	if ((surfaceProperties.maxImageCount > 0) && (desiredSwapchainImages > surfaceProperties.maxImageCount))
	{
		// Application must settle for fewer images than desired.
		desiredSwapchainImages = surfaceProperties.maxImageCount;
	}

	// Figure out a suitable surface transform.
	VkSurfaceTransformFlagBitsKHR preTransform;
	if (surfaceProperties.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		preTransform = surfaceProperties.currentTransform;

	VkSwapchainKHR oldSwapchain = swapchain;

	// Find a supported composite type.
	VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	if (surfaceProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	else if (surfaceProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
		composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	else if (surfaceProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
		composite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
	else if (surfaceProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
		composite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;

	VkSwapchainCreateInfoKHR info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	info.surface = surface;
	info.minImageCount = desiredSwapchainImages;
	info.imageFormat = format.format;
	info.imageColorSpace = format.colorSpace;
	info.imageExtent.width = swapchainSize.width;
	info.imageExtent.height = swapchainSize.height;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.preTransform = preTransform;
	info.compositeAlpha = composite;
	info.presentMode = swapchainPresentMode;
	info.clipped = true;
	info.oldSwapchain = oldSwapchain;

	VK_CHECK(vkCreateSwapchainKHR(device, &info, nullptr, &swapchain));

	if (oldSwapchain != VK_NULL_HANDLE)
		vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

	swapchainDimensions.width = swapchainSize.width;
	swapchainDimensions.height = swapchainSize.height;
	swapchainDimensions.format = format.format;

	uint32_t imageCount;
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));
	swapchainImages.resize(imageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()));

	return RESULT_SUCCESS;
}

void WSIPlatform::getCurrentSwapchain(vector<VkImage> *images, SwapchainDimensions *swapchain)
{
	*images = swapchainImages;
	*swapchain = swapchainDimensions;
}

unsigned WSIPlatform::getNumSwapchainImages() const
{
	return swapchainImages.size();
}

Result WSIPlatform::acquireNextImage(unsigned *image)
{
	auto acquireSemaphore = semaphoreManager->getClearedSemaphore();
	VkResult res = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, image);

	if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		vkQueueWaitIdle(queue);
		semaphoreManager->addClearedSemaphore(acquireSemaphore);

		// Recreate swapchain.
		if (SUCCEEDED(initSwapchain(swapchainDimensions)))
			return RESULT_ERROR_OUTDATED_SWAPCHAIN;
		else
			return RESULT_ERROR_GENERIC;
	}
	else if (res != VK_SUCCESS)
	{
		vkQueueWaitIdle(queue);
		semaphoreManager->addClearedSemaphore(acquireSemaphore);
		return RESULT_ERROR_GENERIC;
	}
	else
	{
		// Signal the underlying context that we're using this backbuffer now.
		// This will also wait for all fences associated with this swapchain image
		// to complete first.
		// When submitting command buffer that writes to swapchain, we need to wait
		// for this semaphore first.
		// Also, delete the older semaphore.
		auto oldSemaphore = pContext->beginFrame(*image, acquireSemaphore);

		// Recycle the old semaphore back into the semaphore manager.
		if (oldSemaphore != VK_NULL_HANDLE)
			semaphoreManager->addClearedSemaphore(oldSemaphore);

		return RESULT_SUCCESS;
	}
}

Result WSIPlatform::presentImage(unsigned index)
{
	VkResult result;
	VkPresentInfoKHR present = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	present.swapchainCount = 1;
	present.pSwapchains = &swapchain;
	present.pImageIndices = &index;
	present.pResults = &result;
	present.waitSemaphoreCount = 1;
	present.pWaitSemaphores = &pContext->getSwapchainReleaseSemaphore();

	VkResult res = vkQueuePresentKHR(queue, &present);

	if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
		return RESULT_ERROR_OUTDATED_SWAPCHAIN;
	else if (res != VK_SUCCESS)
		return RESULT_ERROR_GENERIC;
	else
		return RESULT_SUCCESS;
}
}
