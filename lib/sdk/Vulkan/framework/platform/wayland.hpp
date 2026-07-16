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
 * Wayland windowing backend (xdg-shell, generated via wayland-scanner).
 *
 * The per-backend Platform::get() lives in platform.cpp so that both Wayland
 * and Display backends can coexist in one binary and be selected at runtime.
 */

#ifndef PLATFORM_WAYLAND_HPP
#define PLATFORM_WAYLAND_HPP

#include "platform.hpp"
#include "wsi.hpp"
#include "xdg-shell-client-protocol.h"

namespace VersalGPUSDK
{

/// @brief Holds all Wayland-specific objects and state for window management.
struct WaylandData
{
	wl_display *dpy;
	wl_surface *surf;
	wl_registry *registry;
	wl_compositor *compositor;
	xdg_wm_base *wm_base;
	xdg_surface *xdg_Surface;
	xdg_toplevel *xdg_TopLevel;
	Platform::Status status;
	int fd;
};

/// @brief Wayland-specific implementation of the WSIPlatform interface.
class WaylandPlatform : public WSIPlatform
{
public:
	WaylandPlatform();
	virtual ~WaylandPlatform();

	virtual Result initialize() override;
	virtual SwapchainDimensions getPreferredSwapchain() override;
	virtual Result createWindow(const SwapchainDimensions &swapchain) override;
	virtual Status getWindowStatus() override;
	virtual Result presentImage(unsigned index) override;
	virtual void terminate() override;

private:
	struct WaylandData waylandData;

	Result initWindow();
	void flushWaylandFd();

	virtual VkSurfaceKHR createSurface() override;
};
}

#endif
