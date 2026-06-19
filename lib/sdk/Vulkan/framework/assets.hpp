/*
 * Ported from ssw-mktg/GPU_SDK_Vulkan/framework/assets.hpp.
 *
 * Assets (shaders + textures) are resolved relative to the executable's
 * "assets/" directory (see assets.cpp). The texture loaders mirror the original
 * framework so the texture-based samples port unchanged.
 */

#ifndef FRAMEWORK_ASSETS_HPP
#define FRAMEWORK_ASSETS_HPP

#include "common.hpp"

#include <stdint.h>
#include <vector>

namespace VersalGPUSDK
{
/// @brief Loads a SPIR-V shader module from assets.
/// @param device The Vulkan device.
/// @param pPath Path to the SPIR-V shader, relative to the executable's assets
/// directory.
/// @returns A newly allocated shader module or VK_NULL_HANDLE on error.
VkShaderModule loadShaderModule(VkDevice device, const char *pPath);

/// @brief Loads texture data from assets as VK_FORMAT_R8G8B8A8_UNORM.
///
/// @param      pPath   Path to texture, relative to the assets directory.
/// @param[out] pBuffer Output buffer where RGBA8 pixels are placed.
/// @param[out] pWidth  Width of the loaded texture.
/// @param[out] pHeight Height of the loaded texture.
/// @returns Error code.
Result loadRgba8888TextureFromAsset(const char *pPath, std::vector<uint8_t> *pBuffer, unsigned *pWidth,
                                    unsigned *pHeight);

/// @brief Loads an ASTC texture (astcenc output) from assets.
///
/// @param      pPath   Path to texture, relative to the assets directory.
/// @param[out] pBuffer Output buffer where the ASTC payload is placed.
/// @param[out] pWidth  Width of the loaded texture.
/// @param[out] pHeight Height of the loaded texture.
/// @param[out] pFormat The Vulkan format of the loaded texture.
/// @returns Error code.
Result loadASTCTextureFromAsset(const char *pPath, std::vector<uint8_t> *pBuffer, unsigned *pWidth, unsigned *pHeight,
                                VkFormat *pFormat);
}

#endif
