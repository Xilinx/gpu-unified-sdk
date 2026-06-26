/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "VulkanUtils.hpp"

#include <cstring>

#include "Common/Log.hpp"




namespace vk {




Extensions Extensions::fromList(const std::vector<Item> & items) noexcept
{
	const auto makeProperties = [&items] () -> std::vector<VkExtensionProperties> {
		std::vector<VkExtensionProperties> list;
		list.resize(items.size());
		for (int i = 0; i < int(items.size()); ++i) {
			const Item & item = items[i];
			ASSERT(item.name.size() < VK_MAX_EXTENSION_NAME_SIZE)
			VkExtensionProperties & props = list[i];
			std::strncpy(props.extensionName, item.name.data(), item.name.size());
			props.extensionName[item.name.size()] = '\0';
		}
		return list;
	};
	return Extensions::fromProperties(makeProperties());
}


Extensions Extensions::instanceExtensions(const char * const layerName)
{
	uint32_t count = 0;
	CHECKVK(::vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr))
	std::vector<VkExtensionProperties> properties;
	properties.resize(count);
	CHECKVK(::vkEnumerateInstanceExtensionProperties(layerName, &count, properties.data()))
	return Extensions::fromProperties(std::move(properties));
}


Extensions Extensions::gpuExtensions(const VkPhysicalDevice gpu, const char * const layerName)
{
	uint32_t count = 0;
	CHECKVK(vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, nullptr))
	std::vector<VkExtensionProperties> extensions;
	extensions.resize(count);
	CHECKVK(vkEnumerateDeviceExtensionProperties(gpu, nullptr, &count, extensions.data()))
	return Extensions::fromProperties(std::move(extensions));
}


bool Extensions::contains(const std::string_view & name, const int minSpecVersion) const noexcept
{
	const auto it = std::find_if(list_.begin(), list_.end(),
			[&name] (const Item & i) { return i.name == name; });
	if (it == list_.end()) return false;
	const Item & item = *it;
	return item.specVersion >= minSpecVersion;
}


std::vector<Extensions::Item> Extensions::_makeList(
		const std::vector<VkExtensionProperties> & properties) noexcept
{
	std::vector<Item> items;
	for (const VkExtensionProperties & p : properties) {
		items.push_back(Item{
			.name = p.extensionName,
			.specVersion = int(p.specVersion),
		});
	}
	std::sort(items.begin(), items.end(),
			[] (const Item & a, const Item & b) { return a.name < b.name; });
	return items;
}


std::vector<VkPhysicalDevice> enumerateGpus(const VkInstance instance)
{
	uint32_t count = 0;
	CHECKVK(vkEnumeratePhysicalDevices(instance, &count, nullptr))
	std::vector<VkPhysicalDevice> list;
	list.resize(count);
	CHECKVK(vkEnumeratePhysicalDevices(instance, &count, list.data()))
	return list;
}


std::vector<VkQueueFamilyProperties> getGpuQueueFamilyProperties(const VkPhysicalDevice gpu) noexcept
{
	uint32_t count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, nullptr);
	std::vector<VkQueueFamilyProperties> list(count);
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, list.data());
	return list;
}


std::vector<VkPresentModeKHR> getGpuSurfacePresentModes(const VkPhysicalDevice gpu,
		const VkSurfaceKHR surface)
{
	uint32_t count = 0;
	CHECKVK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &count, nullptr))
	std::vector<VkPresentModeKHR> list(count);
	CHECKVK(vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &count, list.data()))
	return list;
}




} // vk
