/*
 * Ported verbatim from ssw-mktg/GPU_SDK_Vulkan/framework/command_buffer_manager.cpp.
 */

#include "command_buffer_manager.hpp"

namespace VersalGPUSDK
{
CommandBufferManager::CommandBufferManager(VkDevice vkDevice, VkCommandBufferLevel bufferLevel,
                                           unsigned graphicsQueueIndex)
    : device(vkDevice)
    , commandBufferLevel(bufferLevel)
    , count(0)
{
	VkCommandPoolCreateInfo info = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	info.queueFamilyIndex = graphicsQueueIndex;
	info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &pool));
}

CommandBufferManager::~CommandBufferManager()
{
	if (!buffers.empty())
		vkFreeCommandBuffers(device, pool, buffers.size(), buffers.data());
	vkDestroyCommandPool(device, pool, nullptr);
}

void CommandBufferManager::beginFrame()
{
	count = 0;
	vkResetCommandPool(device, pool, 0);
}

VkCommandBuffer CommandBufferManager::requestCommandBuffer()
{
	// Either we recycle a previously allocated command buffer, or create a new
	// one.
	VkCommandBuffer ret = VK_NULL_HANDLE;
	if (count < buffers.size())
	{
		ret = buffers[count++];
	}
	else
	{
		VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		info.commandPool = pool;
		info.level = commandBufferLevel;
		info.commandBufferCount = 1;
		VK_CHECK(vkAllocateCommandBuffers(device, &info, &ret));
		buffers.push_back(ret);

		count++;
	}

	return ret;
}
}
