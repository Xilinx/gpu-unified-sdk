/*
 * Ported verbatim from ssw-mktg/GPU_SDK_Vulkan/framework/math.hpp.
 *
 * GLM convenience header shared by the Vulkan samples, plus the GL->Vulkan
 * projection fix-up. glm is provided by the Yocto SDK sysroot (resolved via
 * find_package(glm)) and exposed by the SdkVulkan target.
 */

#ifndef FRAMEWORK_MATH_HPP
#define FRAMEWORK_MATH_HPP

#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

namespace VersalGPUSDK
{
/// @brief Converts an OpenGL style projection matrix to Vulkan style projection
/// matrix.
///
/// Vulkan has a topLeft clipSpace with [0, 1] depth range instead of [-1, 1].
///
/// GLM outputs projection matrices in GL style clipSpace,
/// perform a simple fix-up step to change the projection to VulkanStyle.
///
/// @param proj Projection matrix
///
/// @returns A Vulkan compatible projection matrix.
inline glm::mat4 vulkanStyleProjection(const glm::mat4 &proj)
{
	using namespace glm;

	// Flip Y in clipspace. X = -1, Y = -1 is topLeft in Vulkan.
	auto mat = scale(mat4(1.0f), vec3(1.0f, -1.0f, 1.0f));

	// Z depth is [0, 1] range instead of [-1, 1].
	mat = scale(mat, vec3(1.0f, 1.0f, 0.5f));
	return translate(mat, vec3(0.0f, 0.0f, 1.0f)) * proj;
}
}

#endif
