/*
 * Ported verbatim from ssw-mktg/GPU_SDK_Vulkan/framework/fence_manager.cpp.
 */

#include "fence_manager.hpp"

namespace VersalGPUSDK
{
FenceManager::FenceManager(VkDevice vkDevice)
    : device(vkDevice)
    , count(0)
{
}

FenceManager::~FenceManager()
{
	beginFrame();
	for (auto &fence : fences)
		vkDestroyFence(device, fence, nullptr);
}

void FenceManager::beginFrame()
{
	// If we have outstanding fences for this swapchain image, wait for them to
	// complete first.
	// Normally, this doesn't really block at all,
	// since we're waiting for old frames to have been completed, but just in
	// case.
	if (count != 0)
	{
		vkWaitForFences(device, count, fences.data(), true, UINT64_MAX);
		vkResetFences(device, count, fences.data());
	}
	count = 0;
}

VkFence FenceManager::requestClearedFence()
{
	if (count < fences.size())
		return fences[count++];

	VkFence fence;
	VkFenceCreateInfo info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	VK_CHECK(vkCreateFence(device, &info, nullptr, &fence));
	fences.push_back(fence);
	count++;
	return fence;
}
}
