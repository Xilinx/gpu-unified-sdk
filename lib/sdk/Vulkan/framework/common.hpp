/*
 * Vulkan sample framework - common definitions.
 *
 * This version includes <vulkan/vulkan.h> directly (the SDK links against the
 * Vulkan loader via Vulkan::Vulkan).
 */

#ifndef FRAMEWORK_COMMON_HPP
#define FRAMEWORK_COMMON_HPP

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>

#define LOGE(...) fprintf(stderr, "ERROR: " __VA_ARGS__)
#define LOGI(...) fprintf(stderr, "INFO: " __VA_ARGS__)

/// @brief Helper macro to test the result of Vulkan calls which can return an
/// error.
#define VK_CHECK(x)                                                                     \
	do                                                                                  \
	{                                                                                   \
		VkResult err = x;                                                               \
		if (err)                                                                        \
		{                                                                               \
			LOGE("Detected Vulkan error %d at %s:%d.\n", int(err), __FILE__, __LINE__); \
			abort();                                                                    \
		}                                                                               \
	} while (0)

#define ASSERT_VK_HANDLE(handle)                                    \
	do                                                              \
	{                                                               \
		if ((handle) == VK_NULL_HANDLE)                             \
		{                                                           \
			LOGE("Handle is NULL at %s:%d.\n", __FILE__, __LINE__); \
			abort();                                                \
		}                                                           \
	} while (0)

namespace VersalGPUSDK
{

/// @brief Generic error codes used throughout the framework and platform.
enum Result
{
	/// Success
	RESULT_SUCCESS = 0,

	/// Generic error without any particular information
	RESULT_ERROR_GENERIC = -1,

	/// Returned by the swapchain when the swapchain is invalid and should be
	/// recreated
	RESULT_ERROR_OUTDATED_SWAPCHAIN = -2,

	/// Generic input/output errors
	RESULT_ERROR_IO = -3,

	/// Memory allocation errors
	RESULT_ERROR_OUT_OF_MEMORY = -4
};

/// @brief Helper macro to determine success of a call.
#define SUCCEEDED(x) ((x) == RESULT_SUCCESS)
/// @brief Helper macro to determine failure of a call.
#define FAILED(x) ((x) != RESULT_SUCCESS)
}

#endif
