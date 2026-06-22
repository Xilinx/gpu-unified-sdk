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
 * @file XLinuxPodium.h
 * @brief This file implements all the functions related to Linux X11 windowing system API's for application.
 *
*******************************************************************************/
#ifndef XLINUXPODIUM_H
#define XLINUXPODIUM_H

#include <cstdlib>
#include <EGL/egl.h>
#include "XPodium.h"
#ifdef ENABLE_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

/**
 * @class XLinuxPodium
 * @brief A concrete implementation of the XPodium class for X11 systems on Linux.
 * 
 * This class provides specific implementations for managing and interacting with
 * windows in an X11 environment on Linux. It extends the XPodium base class and
 * implements methods for window preparation, destruction, status checking, and
 * X11 window creation.
 */
class XLinuxPodium : public XPodium
{
private:
    int windowWidth; ///< Width of the window in pixels.
    int windowHeight; ///< Height of the window in pixels.
    Colormap colormap; ///< Colormap used for the window.
    XVisualInfo *visual; ///< Visual information for the window.
    static XPodium* instance; ///< Singleton instance of the XLinuxPodium class.

    /**
     * @brief Private constructor to enforce the singleton pattern.
     * 
     * The constructor is private to ensure that only one instance of the XLinuxPodium
     * class is created. Use the getHandler() method to access the singleton instance.
     */
    XLinuxPodium(void);

    /**
     * @brief Callback function to wait for the X11 window map event.
     * 
     * This static method is used as a callback to wait for the X11 window to be mapped.
     * 
     * @param display The X11 display connection.
     * @param event Pointer to the XEvent structure.
     * @param windowPointer Pointer to the window pointer.
     * @return True if the window is successfully mapped, false otherwise.
     */
    static Bool wait_for_map(Display *display, XEvent *event, char *windowPointer);

public:
    /**
     * @brief Retrieves the singleton instance of the XLinuxPodium class.
     * 
     * This static method returns the singleton instance of the XLinuxPodium class.
     * If the instance does not exist, it will be created.
     * 
     * @return Pointer to the singleton instance of the XLinuxPodium class.
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
     * @brief Creates an X11 window.
     * 
     * This method initializes and creates an X11 window, setting up the necessary
     * visual information and colormap. It returns true if the window is created successfully,
     * false otherwise.
     * 
     * @return True if the X11 window is created successfully, false otherwise.
     */
    bool createX11Window(void);
};

#endif /* XLINUXPODIUM_H */

