/*******************************************************************************
 * @cond CC
 * Copyright (C) 2024 AMD, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a AMD device, or
 * (b) that interact with a AMD device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AMD CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the AMD shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from AMD.
 *
*******************************************************************************/
/**
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Nirmal         7/06/25         Wayland implementation.
 * 
 * @endcond
 *
*******************************************************************************/
/**
 * @file XWaylandPodium.cpp
 * @brief This file implements all the functions related to Linux Wayland windowing system API's for application.
 *
*******************************************************************************/

#include "XWaylandPodium.h"
#include <cstring>
#include <iostream>

// Wayland listener structures
static const struct wl_registry_listener registry_listener = {
    XWaylandPodium::registry_handler,
    XWaylandPodium::registry_remover
};

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    XWaylandPodium::xdg_wm_base_ping
};

static const struct xdg_surface_listener xdg_surface_listener = {
    XWaylandPodium::xdg_surface_configure
};

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    XWaylandPodium::xdg_toplevel_configure,
    XWaylandPodium::xdg_toplevel_close
};

static const struct wl_pointer_listener pointer_listener = {
    XWaylandPodium::pointer_enter,
    XWaylandPodium::pointer_leave,
    XWaylandPodium::pointer_motion,
    XWaylandPodium::pointer_button,
    XWaylandPodium::pointer_axis
};

static const struct wl_seat_listener seat_listener = {
    XWaylandPodium::seat_capabilities,
    XWaylandPodium::seat_name
};

XPodium* XWaylandPodium::instance = NULL;

XWaylandPodium::XWaylandPodium(void)
    : wl_display(nullptr), wl_compositor(nullptr), wl_surface(nullptr), 
      wl_registry(nullptr), wl_xdg_wm_base(nullptr), wl_xdg_surface(nullptr), 
      wl_xdg_toplevel(nullptr), wl_seat(nullptr), wl_pointer(nullptr),
      wl_egl_window(nullptr), windowWidth(800), windowHeight(600), 
      windowConfigured(false), closeRequested(false), pointerEntered(false)
{
    mouseClick.x = 0;
    mouseClick.y = 0;
}

XPodium* XWaylandPodium::getHandler(void)
{
    if (instance == NULL)
    {
        instance = new XWaylandPodium();
    }
    return instance;
}

void XWaylandPodium::prepareWindow(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    
    if (!initWayland())
    {
        log("Failed to initialize Wayland");
        return;
    }
    
    log("Wayland window prepared successfully: %dx%d", width, height);
}

bool XWaylandPodium::initWayland(void)
{
    // Connect to Wayland display
    wl_display = wl_display_connect(nullptr);
    if (!wl_display)
    {
        log("Failed to connect to Wayland display");
        return false;
    }
    
    // Get registry and add listener
    wl_registry = wl_display_get_registry(wl_display);
    wl_registry_add_listener(wl_registry, &registry_listener, this);
    
    // Dispatch events to get global objects
    wl_display_dispatch(wl_display);
    wl_display_roundtrip(wl_display);
    
    if (!wl_compositor || !wl_xdg_wm_base)
    {
        log("Failed to get Wayland compositor or xdg_wm_base");
        return false;
    }
    
    // Create surface
    wl_surface = wl_compositor_create_surface(wl_compositor);
    if (!wl_surface)
    {
        log("Failed to create Wayland surface");
        return false;
    }
    
    // Create XDG surface
    wl_xdg_surface = xdg_wm_base_get_xdg_surface(wl_xdg_wm_base, wl_surface);
    if (!wl_xdg_surface)
    {
        log("Failed to create XDG surface");
        return false;
    }
    
    xdg_surface_add_listener(wl_xdg_surface, &xdg_surface_listener, this);
    
    // Create XDG toplevel
    wl_xdg_toplevel = xdg_surface_get_toplevel(wl_xdg_surface);
    if (!wl_xdg_toplevel)
    {
        log("Failed to create XDG toplevel");
        return false;
    }
    
    xdg_toplevel_add_listener(wl_xdg_toplevel, &xdg_toplevel_listener, this);
    xdg_toplevel_set_title(wl_xdg_toplevel, "AMD GPU SDK Application");
    
    // Commit surface to trigger initial configure
    wl_surface_commit(wl_surface);
    
    // Wait for initial configure event
    while (!windowConfigured) {
        wl_display_dispatch(wl_display);
    }
    
    // Now create EGL window after surface is configured
    wl_egl_window = wl_egl_window_create(wl_surface, windowWidth, windowHeight);
    
    if (!wl_egl_window)
    {
        log("Failed to create Wayland EGL window");
        return false;
    }
    
    return true;
}

void XWaylandPodium::destroyWindow(void)
{
    if (wl_egl_window)
    {
        wl_egl_window_destroy(wl_egl_window);
        wl_egl_window = nullptr;
    }
    
    if (wl_pointer)
    {
        wl_pointer_destroy(wl_pointer);
        wl_pointer = nullptr;
    }
    
    if (wl_seat)
    {
        wl_seat_destroy(wl_seat);
        wl_seat = nullptr;
    }
    
    if (wl_xdg_toplevel)
    {
        xdg_toplevel_destroy(wl_xdg_toplevel);
        wl_xdg_toplevel = nullptr;
    }
    
    if (wl_xdg_surface)
    {
        xdg_surface_destroy(wl_xdg_surface);
        wl_xdg_surface = nullptr;
    }
    
    if (wl_surface)
    {
        wl_surface_destroy(wl_surface);
        wl_surface = nullptr;
    }
    
    if (wl_xdg_wm_base)
    {
        xdg_wm_base_destroy(wl_xdg_wm_base);
        wl_xdg_wm_base = nullptr;
    }
    
    if (wl_compositor)
    {
        wl_compositor_destroy(wl_compositor);
        wl_compositor = nullptr;
    }
    
    if (wl_registry)
    {
        wl_registry_destroy(wl_registry);
        wl_registry = nullptr;
    }
    
    if (wl_display)
    {
        wl_display_disconnect(wl_display);
        wl_display = nullptr;
    }
    
    log("Wayland window destroyed");
}

XPodium::WindowStatus XWaylandPodium::checkWindow(void)
{
    if (closeRequested)
    {
        return XPodium::WINDOW_EXIT;
    }
    
    processWaylandEvents();
    
    // Check if we have a pending mouse click
    if (mouseClick.x != 0 || mouseClick.y != 0)
    {
        // Reset click position after returning status
        mouseClick.x = 0;
        mouseClick.y = 0;
        return XPodium::WINDOW_CLICK;
    }
    
    return XPodium::WINDOW_IDLE;
}

void XWaylandPodium::processWaylandEvents(void)
{
    // Process any pending events without blocking
    wl_display_dispatch_pending(wl_display);
    wl_display_flush(wl_display);
}

// Wayland event handlers implementation 
/*
void XWaylandPodium::registry_handler(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    
    if (strcmp(interface, "wl_compositor") == 0)
    {
        podium->wl_compositor = static_cast<wl_compositor*>(
            wl_registry_bind(registry, id, &wl_compositor_interface, 4));
    }
    else if (strcmp(interface, "xdg_wm_base") == 0)
    {
        podium->wl_xdg_wm_base = static_cast<xdg_wm_base*>(
            wl_registry_bind(registry, id, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(podium->wl_xdg_wm_base, &xdg_wm_base_listener, podium);
    }
    else if (strcmp(interface, "wl_seat") == 0)
    {
        podium->wl_seat = static_cast<wl_seat*>(
            wl_registry_bind(registry, id, &wl_seat_interface, 1));
        wl_seat_add_listener(podium->wl_seat, &seat_listener, podium);
    }
} 
*/
void XWaylandPodium::registry_handler(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    
    if (strcmp(interface, "wl_compositor") == 0)
    {
        podium->wl_compositor = static_cast<struct wl_compositor*>(
            wl_registry_bind(registry, id, &wl_compositor_interface, 4));
    }
    else if (strcmp(interface, "xdg_wm_base") == 0)
    {
        podium->wl_xdg_wm_base = static_cast<struct xdg_wm_base*>(
            wl_registry_bind(registry, id, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(podium->wl_xdg_wm_base, &xdg_wm_base_listener, podium);
    }
    else if (strcmp(interface, "wl_seat") == 0)
    {
        podium->wl_seat = static_cast<struct wl_seat*>(
            wl_registry_bind(registry, id, &wl_seat_interface, 1));
        wl_seat_add_listener(podium->wl_seat, &seat_listener, podium);
    }
}

void XWaylandPodium::registry_remover(void* data, struct wl_registry* registry, uint32_t id)
{
    // Handle registry object removal if needed
}

void XWaylandPodium::xdg_wm_base_ping(void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

void XWaylandPodium::xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    xdg_surface_ack_configure(xdg_surface, serial);
    podium->windowConfigured = true;
}

void XWaylandPodium::xdg_toplevel_configure(void* data, struct xdg_toplevel* xdg_toplevel, int32_t width, int32_t height, struct wl_array* states)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    
    if (width > 0 && height > 0)
    {
        podium->windowWidth = width;
        podium->windowHeight = height;
        
        if (podium->wl_egl_window)
        {
            wl_egl_window_resize(podium->wl_egl_window, width, height, 0, 0);
        }
    }
}

void XWaylandPodium::xdg_toplevel_close(void* data, struct xdg_toplevel* xdg_toplevel)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    podium->closeRequested = true;
}

void XWaylandPodium::seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    
    if (capabilities & WL_SEAT_CAPABILITY_POINTER)
    {
        podium->wl_pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(podium->wl_pointer, &pointer_listener, podium);
    }
    else if (podium->wl_pointer)
    {
        wl_pointer_destroy(podium->wl_pointer);
        podium->wl_pointer = nullptr;
    }
}

void XWaylandPodium::seat_name(void* data, struct wl_seat* seat, const char* name)
{
    // Seat name information - not currently used
}

void XWaylandPodium::pointer_enter(void* data, struct wl_pointer* pointer, uint32_t serial, struct wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    podium->pointerEntered = true;
}

void XWaylandPodium::pointer_leave(void* data, struct wl_pointer* pointer, uint32_t serial, struct wl_surface* surface)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    podium->pointerEntered = false;
}

void XWaylandPodium::pointer_motion(void* data, struct wl_pointer* pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    // Mouse motion - can be used to track cursor position if needed
    // XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    // Convert fixed point to pixels: wl_fixed_to_double(sx), wl_fixed_to_double(sy)
}

void XWaylandPodium::pointer_button(void* data, struct wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    XWaylandPodium* podium = static_cast<XWaylandPodium*>(data);
    
    if (state == WL_POINTER_BUTTON_STATE_PRESSED)
    {
        // Store the click position (could get actual coordinates from pointer_motion)
        // For now, just mark that a click occurred
        podium->mouseClick.x = 1;  // Non-zero to indicate click occurred
        podium->mouseClick.y = 1;
    }
}

void XWaylandPodium::pointer_axis(void* data, struct wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    // Mouse wheel/scroll events - not currently used
}