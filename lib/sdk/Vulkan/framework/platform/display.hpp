/* Copyright (c) 2016-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Modifications Copyright (C) 2024-2026 Advanced Micro Devices, Inc.  All rights reserved.
 */

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
