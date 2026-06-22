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
 * @file XEGLIntf.h
 *
 * @brief This file implements all the functions related to EGL bindings.
 *
*******************************************************************************/
#ifndef XEGLINTF_H
#define XEGLINTF_H

#include <EGL/egl.h>
#include <EGL/eglext.h>

/**
 * @class CoreEGL
 * @brief A class to handle EGL (Embedded-System Graphics Library) operations.
 * 
 * This class provides static methods and attributes for managing EGL initialization,
 * configuration, and context setup. It supports setting EGL samples and handling
 * different versions of OpenGL ES.
 */
class CoreEGL
{
private:
    /**
     * @brief Finds an appropriate EGLConfig based on the strict match requirement.
     * 
     * This static method searches for a suitable EGLConfig that matches the given
     * criteria. It is used internally to configure the EGL environment.
     * 
     * @param strictMatch Boolean indicating whether to enforce strict matching of attributes.
     * @return The found EGLConfig.
     */
    static EGLConfig findConfig(bool strictMatch);

    /// Array of EGL configuration attributes.
    static EGLint configAttributes [];
    
    /// Array of EGL context attributes.
    static EGLint contextAttributes [];
    
    /// Array of EGL window attributes.
    static EGLint windowAttributes [];

public:
    /**
     * @brief Sets the number of samples for EGL to use.
     * 
     * This static method configures the number of samples (for anti-aliasing) that EGL
     * should use for rendering. It affects the quality of rendered images.
     * 
     * @param requiredEGLSamples The number of samples required for anti-aliasing.
     */
    static void setEGLSamples(EGLint requiredEGLSamples);

    /**
     * @enum OpenGLESVersion
     * @brief Enumeration of supported OpenGL ES versions.
     * 
     * This enumeration defines the different versions of OpenGL ES that can be requested
     * during EGL initialization.
     */
    enum OpenGLESVersion {
        OPENGLES1,   ///< OpenGL ES 1.x
        OPENGLES2,   ///< OpenGL ES 2.x
        OPENGLES3,   ///< OpenGL ES 3.x
        OPENGLES31   ///< OpenGL ES 3.1
    };

    /// The EGL display connection.
    static EGLDisplay display;

    /// The EGL rendering context.
    static EGLContext context;

    /// The EGL configuration used for creating the context.
    static EGLConfig config;

    /// The EGL surface used for rendering.
    static EGLSurface surface;

    /**
     * @brief Initializes the EGL environment.
     * 
     * This static method sets up the EGL environment based on the requested OpenGL ES version.
     * It configures the display, context, and surface for rendering.
     * 
     * @param requestedAPIVersion The version of OpenGL ES to initialize (using the OpenGLESVersion enum).
     */
    static void initializeEGL(OpenGLESVersion requestedAPIVersion);

    /**
     * @brief Terminates the EGL environment.
     * 
     * This static method cleans up and terminates the EGL environment, releasing resources
     * and destroying the display connection, context, and surface.
     */
    static void terminateEGL(void);
};

#endif /* XEGLINTF_H */
