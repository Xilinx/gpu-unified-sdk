/*
 * Direct-to-display (VK_KHR_display) windowing backend interface.
 *
 * The per-backend Platform::get() lives in platform.cpp (runtime selection);
 * assets are handled separately in framework/assets.{hpp,cpp}.
 */

#ifndef PLATFORM_DISPLAY_HPP
#define PLATFORM_DISPLAY_HPP

#include "platform.hpp"
#include "wsi.hpp"

namespace VersalGPUSDK
{

/// @brief The generic embedded platform, which makes use of VK_KHR_display
/// extension. Inherits from WSIPlatform.
class DisplayPlatform : public WSIPlatform
{
public:
	DisplayPlatform() = default;

	virtual SwapchainDimensions getPreferredSwapchain() override;
	virtual Result createWindow(const SwapchainDimensions &swapchain) override;
	virtual Status getWindowStatus() override;

private:
	Platform::Status status = STATUS_RUNNING;
	virtual VkSurfaceKHR createSurface() override;

	unsigned preferredWidth = 0;
	unsigned preferredHeight = 0;
};
}

#endif
