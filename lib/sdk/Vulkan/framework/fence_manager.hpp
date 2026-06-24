#ifndef FRAMEWORK_FENCE_MANAGER_HPP
#define FRAMEWORK_FENCE_MANAGER_HPP

#include "common.hpp"
#include <vector>

namespace VersalGPUSDK
{
/// @brief The FenceManager keeps track of fences which in turn are used to keep
/// track of GPU progress.
///
/// Whenever we submit work to the GPU, it is our responsibility to make sure
/// that the GPU is done
/// using our resources before we modify or delete them.
/// To implement this, we therefore use VkFences to keep track of all
/// vkQueueSubmits.
class FenceManager
{
public:
	/// @brief Constructor
	/// @param device The Vulkan device
	FenceManager(VkDevice device);

	/// @brief Destructor
	~FenceManager();

	/// @brief Begins the frame. Waits for GPU to trigger all outstanding fences.
	/// After begin frame returns, it is safe to reuse or delete resources which
	/// were used previously.
	///
	/// We wait for fences which completes N frames earlier, so we do not stall,
	/// waiting
	/// for all GPU work to complete before this returns.
	void beginFrame();

	/// @brief Called internally by the Context whenever submissions to GPU
	/// happens.
	VkFence requestClearedFence();

	/// @brief Gets the number of fences which are inFlight on the GPU.
	/// @returns The number of fences which can be waited for.
	unsigned getActiveFenceCount() const
	{
		return count;
	}

	/// @brief Gets an array for the fences which are inFlight on the GPU.
	/// @returns Array of waitable fences. Call @ref getActiveFenceCount for the
	/// number of fences.
	VkFence *getActiveFences()
	{
		return fences.data();
	}

private:
	VkDevice device = VK_NULL_HANDLE;
	std::vector<VkFence> fences;
	unsigned count = 0;
};
}

#endif
