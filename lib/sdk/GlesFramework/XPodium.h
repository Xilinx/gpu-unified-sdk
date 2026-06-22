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
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 * @endcond
 *
*******************************************************************************/
/**
 *
 * @file XPodium.h
 *
 * @brief This file implements all the functions related to podium baseclass.
 *
*******************************************************************************/
#ifndef XPodium_H
#define XPodium_H

#include "XEGLIntf.h"
#include "XCVector.h"
#include <cstdio>
#include "Xfbdev_window.h"

#ifdef ENABLE_X11
#include <X11/Xlib.h>  // ✅ Required for 'Window' and 'Display'
#endif

#ifdef ENABLE_WAYLAND
#include <wayland-client.h>
#include <wayland-egl.h>
#endif

/**
 * @class XPodium
 * @brief Abstract base class for handling different types of podium windows.
 * 
 * This class defines the interface and common functionalities for managing and interacting 
 * with podium windows. Derived classes should implement specific window management details
 * for different platforms or windowing systems.
 */
class XPodium
{
public:
    /**
     * @enum WindowStatus
     * @brief Enum representing the possible statuses of a window.
     */
    enum WindowStatus {
        WINDOW_IDLE, ///< The window is idle and not actively processing events.
        WINDOW_EXIT, ///< The window is requesting to exit.
        WINDOW_CLICK ///< The window has detected a mouse click event.
    };

    CVec2 mouseClick; ///< The position of the last mouse click.
    fbdev_window *Fwindow; ///< Pointer to the framebuffer device window object.

    #ifdef ENABLE_X11
    Window window; ///< X Window System window identifier.
    Display* display; ///< Pointer to the X Window System display connection.
    #endif

    #ifdef ENABLE_WAYLAND
    struct wl_display* wl_display; ///< Wayland display connection.
    struct wl_egl_window* wl_egl_window; ///< Wayland EGL window.
    #endif

    /**
     * @brief Pure virtual function to prepare the window with specified dimensions.
     * 
     * Derived classes must implement this function to initialize and configure
     * the window with the given width and height.
     * 
     * @param width The width of the window in pixels.
     * @param height The height of the window in pixels.
     */
    virtual void prepareWindow(int width, int height) = 0;

    /**
     * @brief Pure virtual function to check the current status of the window.
     * 
     * Derived classes must implement this function to return the current status
     * of the window, which indicates whether it is idle, requesting to exit, 
     * or has detected a mouse click.
     * 
     * @return The current status of the window as a WindowStatus enum value.
     */
    virtual WindowStatus checkWindow(void) = 0;

    /**
     * @brief Pure virtual function to destroy and clean up the window.
     * 
     * Derived classes must implement this function to properly release resources
     * and destroy the window.
     */
    virtual void destroyWindow(void) = 0;

    /**
     * @brief Logs a formatted message to the standard output.
     * 
     * This static method provides a way to log messages with formatted output.
     * 
     * @param format The format string, as used in printf-style functions.
     * @param ... Additional arguments, corresponding to the format string.
     */
    static void log(const char* format, ...);

    /**
     * @brief Gets the singleton instance of the XPodium handler.
     * 
     * This static method returns a pointer to the singleton instance of
     * the XPodium handler. If the instance does not exist, it will be created.
     * 
     * @return Pointer to the XPodium handler instance.
     */
    static XPodium* getHandler();
};

#ifdef ENABLE_FBDEV
#include "XLinuxTarget.h"
#endif

#ifdef ENABLE_X11
#include "XLinuxPodium.h"
#endif

#ifdef ENABLE_WAYLAND
#include "XWaylandPodium.h"
#endif

#endif /* XPodium_H */
