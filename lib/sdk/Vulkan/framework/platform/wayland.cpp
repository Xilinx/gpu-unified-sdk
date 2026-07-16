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
 * Wayland windowing backend (xdg-shell based window management).
 *  - Platform::get() is defined in platform.cpp (runtime backend selection).
 *  - The surface is created by calling vkCreateWaylandSurfaceKHR directly
 *    (loader-exported).
 */

#include "wayland.hpp"

#include <wayland-client.h>
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan_wayland.h>

#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace VersalGPUSDK
{
	VkSurfaceKHR WaylandPlatform::createSurface()
	{
		if (FAILED(initWindow()))
			return VK_NULL_HANDLE;

		VkSurfaceKHR surface;
		VkWaylandSurfaceCreateInfoKHR info = {VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR};
		info.display = waylandData.dpy;
		info.surface = waylandData.surf;

		VK_CHECK(vkCreateWaylandSurfaceKHR(instance, &info, nullptr, &surface));
		return surface;
	}

	WaylandPlatform::WaylandPlatform()
	{
		memset(&waylandData, 0, sizeof(waylandData));
		waylandData.fd = -1;
	}

	// Wayland callbacks
	static void registryHandleGlobal(void *data, wl_registry *reg, uint32_t id, const char *interface, uint32_t version)
	{
		(void)version;
		WaylandData *waylandData = static_cast<WaylandData *>(data);
		if (!strcmp(interface, "wl_compositor"))
			waylandData->compositor = static_cast<wl_compositor *>(wl_registry_bind(reg, id, &wl_compositor_interface, 1));
		else if (!strcmp(interface, "xdg_wm_base"))
			waylandData->wm_base = static_cast<xdg_wm_base *>(wl_registry_bind(reg, id, &xdg_wm_base_interface, 1));
	}

	static void registryHandleGlobalRemove(void *, wl_registry *, uint32_t)
	{
	}

	static const wl_registry_listener registryListener = {
		registryHandleGlobal,
		registryHandleGlobalRemove,
	};

	// XDG WM Base callback
	static void xdgWmBasePing(void *, xdg_wm_base *wmBase, uint32_t serial)
	{
		xdg_wm_base_pong(wmBase, serial);
	}

	static const xdg_wm_base_listener xdgWmBaseListener = {
		xdgWmBasePing,
	};

	// XDG Surface callback
	static void xdgSurfaceConfigure(void *, xdg_surface *xdgSurface, uint32_t serial)
	{
		xdg_surface_ack_configure(xdgSurface, serial);
	}

	static const xdg_surface_listener xdgSurfaceListener = {
		xdgSurfaceConfigure,
	};

	// XDG Toplevel callbacks
	static void xdgToplevelConfigure(void *, xdg_toplevel *, int32_t width, int32_t height, wl_array *)
	{
		if (width > 0 && height > 0)
			LOGI("Wayland: Surface size: %d x %d.\n", width, height);
	}

	static void xdgToplevelClose(void *data, xdg_toplevel *)
	{
		WaylandData *waylandData = static_cast<WaylandData *>(data);
		waylandData->status = Platform::STATUS_TEARDOWN;
	}

	static const xdg_toplevel_listener xdgToplevelListener = {
		xdgToplevelConfigure,
		xdgToplevelClose,
	};

	Platform::Status WaylandPlatform::getWindowStatus()
	{
		return waylandData.status;
	}

	void WaylandPlatform::flushWaylandFd()
	{
		pollfd fd = {0};
		wl_display_dispatch_pending(waylandData.dpy);
		wl_display_flush(waylandData.dpy);

		fd.fd = waylandData.fd;
		fd.events = POLLIN | POLLOUT | POLLERR | POLLHUP;

		if (poll(&fd, 1, 0) > 0)
		{
			if (fd.revents & (POLLERR | POLLHUP))
			{
				close(waylandData.fd);
				waylandData.fd = -1;
				waylandData.status = STATUS_TEARDOWN;
			}

			if (fd.revents & POLLIN)
				wl_display_dispatch(waylandData.dpy);
			if (fd.revents & POLLOUT)
				wl_display_flush(waylandData.dpy);
		}
	}

	Result WaylandPlatform::initialize()
	{
		waylandData.dpy = wl_display_connect(nullptr);
		if (!waylandData.dpy)
			return RESULT_ERROR_IO;

		waylandData.registry = wl_display_get_registry(waylandData.dpy);
		wl_registry_add_listener(waylandData.registry, &registryListener, &waylandData);
		wl_display_roundtrip(waylandData.dpy);

		if (!waylandData.compositor || !waylandData.wm_base)
			return RESULT_ERROR_GENERIC;

		// Add listener for xdg_wm_base
		xdg_wm_base_add_listener(waylandData.wm_base, &xdgWmBaseListener, &waylandData);

		waylandData.fd = wl_display_get_fd(waylandData.dpy);
		if (waylandData.fd < 0)
			return RESULT_ERROR_IO;

		waylandData.status = STATUS_RUNNING;

		return WSIPlatform::initialize();
	}

	Platform::SwapchainDimensions WaylandPlatform::getPreferredSwapchain()
	{
		SwapchainDimensions chain = {1280, 720, VK_FORMAT_B8G8R8A8_UNORM};
		return chain;
	}

	Result WaylandPlatform::initWindow()
	{
		waylandData.surf = wl_compositor_create_surface(waylandData.compositor);
		if (!waylandData.surf)
			return RESULT_ERROR_GENERIC;

		waylandData.xdg_Surface = xdg_wm_base_get_xdg_surface(waylandData.wm_base, waylandData.surf);
		if (!waylandData.xdg_Surface)
			return RESULT_ERROR_GENERIC;

		xdg_surface_add_listener(waylandData.xdg_Surface, &xdgSurfaceListener, &waylandData);

		waylandData.xdg_TopLevel = xdg_surface_get_toplevel(waylandData.xdg_Surface);
		if (!waylandData.xdg_TopLevel)
			return RESULT_ERROR_GENERIC;

		xdg_toplevel_add_listener(waylandData.xdg_TopLevel, &xdgToplevelListener, &waylandData);
		xdg_toplevel_set_title(waylandData.xdg_TopLevel, "GPU SDK - AMD Versal");
		xdg_toplevel_set_app_id(waylandData.xdg_TopLevel, "gpu-sdk-amd-versal");

		// Commit the surface to make it visible
		wl_surface_commit(waylandData.surf);

		flushWaylandFd();
		return RESULT_SUCCESS;
	}

	Result WaylandPlatform::createWindow(const SwapchainDimensions &swapchain)
	{
		if (!waylandData.dpy)
			return RESULT_ERROR_GENERIC;

		return initVulkan(swapchain, {"VK_KHR_surface", "VK_KHR_wayland_surface"}, {"VK_KHR_swapchain"});
	}

	void WaylandPlatform::terminate()
	{
		// Tear down WSI resources before we destroy Wayland.
		WSIPlatform::terminate();

		if (waylandData.dpy)
		{
			if (waylandData.fd >= 0)
				close(waylandData.fd);

			if (waylandData.xdg_TopLevel)
				xdg_toplevel_destroy(waylandData.xdg_TopLevel);
			if (waylandData.xdg_Surface)
				xdg_surface_destroy(waylandData.xdg_Surface);
			if (waylandData.surf)
				wl_surface_destroy(waylandData.surf);
			if (waylandData.wm_base)
				xdg_wm_base_destroy(waylandData.wm_base);
			if (waylandData.compositor)
				wl_compositor_destroy(waylandData.compositor);
			if (waylandData.registry)
				wl_registry_destroy(waylandData.registry);
			if (waylandData.dpy)
				wl_display_disconnect(waylandData.dpy);

			waylandData.dpy = nullptr;
		}
	}

	WaylandPlatform::~WaylandPlatform()
	{
		terminate();
	}

	Result WaylandPlatform::presentImage(unsigned index)
	{
		Result res = WSIPlatform::presentImage(index);
		flushWaylandFd();
		return res;
	}
}
