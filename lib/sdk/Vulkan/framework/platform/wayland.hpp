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
