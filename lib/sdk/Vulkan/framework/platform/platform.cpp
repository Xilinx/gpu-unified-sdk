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
 * Runtime backend selection.
 *
 * To support both VK_KHR_wayland_surface and VK_KHR_display in a single binary
 * (selectable at runtime), this single Platform::get() chooses the backend
 * based on Platform::setBackend() (set from main() via --wayland / --display)
 * and otherwise auto-detects from the WAYLAND_DISPLAY environment variable.
 */

#include "display.hpp"
#include "platform.hpp"
#include "wayland.hpp"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>

namespace VersalGPUSDK
{

// Backend override set before the first Platform::get() call. 0 = auto.
static int g_backendOverride = 0; // 1 = wayland, 2 = display

void platformSetBackend(const char *name)
{
	if (name && strcmp(name, "wayland") == 0)
		g_backendOverride = 1;
	else if (name && strcmp(name, "display") == 0)
		g_backendOverride = 2;
	else
		g_backendOverride = 0;
}

// Auto-detect: a Wayland compositor is reachable if WAYLAND_DISPLAY is set, or a
// wayland-* socket exists under XDG_RUNTIME_DIR (some minimal target shells run
// under Weston without WAYLAND_DISPLAY exported).
static bool waylandSocketAvailable()
{
	const char *wl = getenv("WAYLAND_DISPLAY");
	if (wl != nullptr && wl[0] != '\0')
		return true;

	const char *runtimeDir = getenv("XDG_RUNTIME_DIR");
	if (runtimeDir == nullptr || runtimeDir[0] == '\0')
		return false;

	DIR *dir = opendir(runtimeDir);
	if (dir == nullptr)
		return false;

	bool found = false;
	while (dirent *entry = readdir(dir))
	{
		if (strncmp(entry->d_name, "wayland-", 8) == 0)
		{
			found = true;
			break;
		}
	}
	closedir(dir);
	return found;
}

static bool useWayland()
{
	if (g_backendOverride == 1)
		return true;
	if (g_backendOverride == 2)
		return false;

	return waylandSocketAvailable();
}

Platform &Platform::get()
{
	// Initialized on first call. Only the selected backend's singleton is
	// constructed.
	if (useWayland())
	{
		static WaylandPlatform singleton;
		return singleton;
	}
	else
	{
		static DisplayPlatform singleton;
		return singleton;
	}
}
}
