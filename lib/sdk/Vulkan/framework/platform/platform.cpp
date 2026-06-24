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
