#ifndef FRAMEWORK_COMMAND_BUFFER_MANAGER_HPP
#define FRAMEWORK_COMMAND_BUFFER_MANAGER_HPP

#include "common.hpp"
#include <vector>

namespace VersalGPUSDK
{
/// @brief The command buffer allocates command buffers and recycles them for
/// us.
/// This gives us a convenient interface where we can request command buffers
/// for use when rendering.
/// The manager is not thread-safe and for rendering in multiple threads,
/// multiple per-thread managers
/// should be used.
class CommandBufferManager
{
public:
	/// @brief Constructor
	/// @param device             The Vulkan device
	/// @param bufferLevel        The command buffer level to use,
	///                           either `VK_COMMAND_BUFFER_LEVEL_PRIMARY` or
	///                           `VK_COMMAND_BUFFER_LEVEL_SECONDARY`.
	/// @param graphicsQueueIndex The Vulkan queue family index for where we can
	/// submit graphics work.
	CommandBufferManager(VkDevice device, VkCommandBufferLevel bufferLevel, unsigned graphicsQueueIndex);

	/// @brief Destructor
	~CommandBufferManager();

	/// @brief Requests a fresh or recycled command buffer which is in the reset
	/// state.
	VkCommandBuffer requestCommandBuffer();

	/// @brief Begins the frame. When this is called,
	/// all command buffers managed by this class are assumed to be recycleable.
	void beginFrame();

private:
	VkDevice device = VK_NULL_HANDLE;
	VkCommandPool pool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> buffers;
	VkCommandBufferLevel commandBufferLevel;
	unsigned count = 0;
};
}

#endif
