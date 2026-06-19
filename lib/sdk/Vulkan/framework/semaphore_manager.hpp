/*
 * Ported verbatim from ssw-mktg/GPU_SDK_Vulkan/framework/semaphore_manager.hpp.
 */

#ifndef FRAMEWORK_SEMAPHORE_MANAGER_HPP
#define FRAMEWORK_SEMAPHORE_MANAGER_HPP

#include "common.hpp"
#include <vector>

namespace VersalGPUSDK
{
/// @brief The SemaphoreManager keeps track of semaphores
///
/// This class is mostly used by the WSI implementation so we can recycle
/// semaphores.
class SemaphoreManager
{
public:
	/// @brief Constructor
	/// @param device The Vulkan device
	SemaphoreManager(VkDevice device);

	/// @brief Destructor
	~SemaphoreManager();

	/// @brief Takes ownership of a recycled semaphore or creates a new one.
	/// @returns A VkSemaphore. This semaphore is owned by the API user and must
	/// be destroyed or given back to the semaphore manager.
	VkSemaphore getClearedSemaphore();

	/// @brief Gives ownership of a cleared semaphore to the semaphore manager.
	/// @param semaphore A cleared semaphore. The API user relinquishes ownership
	/// of the semaphore.
	void addClearedSemaphore(VkSemaphore semaphore);

private:
	VkDevice device = VK_NULL_HANDLE;
	std::vector<VkSemaphore> recycledSemaphores;
};
}

#endif
