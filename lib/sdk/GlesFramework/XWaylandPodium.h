/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/**
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R        8/20/24         Cmake + Doxygen.
 * 3.0   Assistant      7/06/25         Wayland implementation.
 * 
 *
*******************************************************************************/
/**
 * @file XWaylandPodium.h
 * @brief This file implements all the functions related to Linux Wayland windowing system API's for application.
 *
*******************************************************************************/
#ifndef XWAYLANDPODIUM_H
#define XWAYLANDPODIUM_H

#include <cstdlib>
#include <EGL/egl.h>
#include "XPodium.h"

#ifdef ENABLE_WAYLAND
#include <wayland-client.h>
#include <wayland-egl.h>
extern "C" {
#include "xdg-shell-client-protocol.h"
}
#endif

/**
 * @class XWaylandPodium
 * @brief A concrete implementation of the XPodium class for Wayland systems on Linux.
 * 
 * This class provides specific implementations for managing and interacting with
 * windows in a Wayland environment on Linux. It extends the XPodium base class and
 * implements methods for window preparation, destruction, status checking, and
 * Wayland window creation.
 */
class XWaylandPodium : public XPodium
{
private:
    // Wayland components
    struct wl_display* wl_display;
    struct wl_compositor* wl_compositor;
    struct wl_surface* wl_surface;
    struct wl_registry* wl_registry;
    struct xdg_wm_base* wl_xdg_wm_base;
    struct xdg_surface* wl_xdg_surface;
    struct xdg_toplevel* wl_xdg_toplevel;
    struct wl_seat* wl_seat;
    struct wl_pointer* wl_pointer;

    // EGL/Window components
    struct wl_egl_window* wl_egl_window;
    
    // Window properties
    int windowWidth; ///< Width of the window in pixels.
    int windowHeight; ///< Height of the window in pixels.
    bool windowConfigured; ///< Flag indicating if the window has been configured.
    bool closeRequested; ///< Flag indicating if window close was requested.
    bool pointerEntered; ///< Flag indicating if pointer is inside window.
    
    static XPodium* instance; ///< Singleton instance of the XWaylandPodium class.

    /**
     * @brief Private constructor to enforce the singleton pattern.
     * 
     * The constructor is private to ensure that only one instance of the XWaylandPodium
     * class is created. Use the getHandler() method to access the singleton instance.
     */
    XWaylandPodium(void);

    /**
     * @brief Initialize Wayland connection and create window.
     * 
     * This method handles the Wayland-specific initialization including connecting
     * to the display, getting required interfaces, and creating the window surface.
     * 
     * @return True if Wayland initialization is successful, false otherwise.
     */
    bool initWayland(void);

    /**
     * @brief Process pending Wayland events.
     * 
     * This method dispatches any pending events from the Wayland compositor,
     * such as pointer events, configure events, etc.
     */
    void processWaylandEvents(void);

public:
    /**
     * @brief Retrieves the singleton instance of the XWaylandPodium class.
     * 
     * This static method returns the singleton instance of the XWaylandPodium class.
     * If the instance does not exist, it will be created.
     * 
     * @return Pointer to the singleton instance of the XWaylandPodium class.
     */
    static XPodium* getHandler(void);

    /**
     * @brief Prepares the window with specified dimensions.
     * 
     * This method initializes and configures the window with the given width and height.
     * It overrides the pure virtual function from the XPodium base class.
     * 
     * @param width The width of the window in pixels.
     * @param height The height of the window in pixels.
     */
    virtual void prepareWindow(int width, int height);

    /**
     * @brief Destroys and cleans up the window.
     * 
     * This method releases resources and destroys the window. It overrides the pure
     * virtual function from the XPodium base class.
     */
    virtual void destroyWindow(void);

    /**
     * @brief Checks the current status of the window.
     * 
     * This method checks and returns the current status of the window, such as whether
     * it is idle, requesting to exit, or has detected a mouse click. It overrides the
     * pure virtual function from the XPodium base class.
     * 
     * @return The current status of the window as a WindowStatus enum value.
     */
    virtual WindowStatus checkWindow(void);

    /**
     * @brief Gets the native Wayland EGL window.
     * 
     * This method returns the Wayland EGL window that can be used for EGL surface creation.
     * 
     * @return Pointer to the Wayland EGL window.
     */
    struct wl_egl_window* getEGLWindow(void) const { return wl_egl_window; }

    /**
     * @brief Gets the native Wayland display.
     * 
     * This method returns the Wayland display connection.
     * 
     * @return Pointer to the Wayland display.
     */
    struct wl_display* getWaylandDisplay(void) const { return wl_display; }

public:
    // Wayland event handlers (must be public to be used as C callbacks)
    static void registry_handler(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version);
    static void registry_remover(void* data, struct wl_registry* registry, uint32_t id);
    static void xdg_wm_base_ping(void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial);
    static void xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial);
    static void xdg_toplevel_configure(void* data, struct xdg_toplevel* xdg_toplevel, int32_t width, int32_t height, struct wl_array* states);
    static void xdg_toplevel_close(void* data, struct xdg_toplevel* xdg_toplevel);
    static void pointer_enter(void* data, struct wl_pointer* pointer, uint32_t serial, struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy);
    static void pointer_leave(void* data, struct wl_pointer* pointer, uint32_t serial, struct wl_surface* surface);
    static void pointer_motion(void* data, struct wl_pointer* pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy);
    static void pointer_button(void* data, struct wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    static void pointer_axis(void* data, struct wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
    static void seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities);
    static void seat_name(void* data, struct wl_seat* seat, const char* name);
};

#endif /* XWAYLANDPODIUM_H */