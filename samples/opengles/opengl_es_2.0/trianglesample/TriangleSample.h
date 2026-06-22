/*******************************************************************************
 *
 * Copyright (C) 2023 Xilinx, Inc.  All rights reserved.
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
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 *******************************************************************************/
/******************************************************************************/
/**
 *
 * @file BasicTriangle.h
 *
 * This is an test example for OpenGL rendering using this SDK.
 *
 * @note        None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        -----------------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * </pre>
 *
 *******************************************************************************/
/******************************* Header Files ********************************/

#define GLES_VERSION 2

#include <GLES2/gl2.h>
#include <string>

using namespace std;

/** @brief Screen width in pixels for the rendering window */
#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

/** @brief Screen height in pixels for the rendering window */
#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

/** @brief Path to font shader resources for text rendering.
 *  Staged next to the executable by the build (see add_gles_sample). */
string fontResourceDirectory = "FontShaders/";

/** @brief Path to sample-specific shader and asset files */
string resourceDirectory = "Supportingfiles/";

/** @brief Filename of the vertex shader source code */
string vertexShaderFilename = "TriangleSample.vert";

/** @brief Filename of the fragment shader source code */
string fragmentShaderFilename = "TriangleSample.frag";

/** @brief OpenGL shader program object ID */
GLuint programID;

/** @brief Vertex attribute location for position data */
GLint iLocPosition = -1;

/** @brief Vertex attribute location for color data */
GLint iLocFillColor = -1;

/** @brief Uniform location for model-view-projection matrix */
GLint iLocMVP = -1;

/**
 * @brief Triangle vertex positions in 3D space
 * 
 * Defines a triangle with vertices forming an isosceles triangle
 * centered around the origin. Each vertex has (x, y, z) coordinates:
 * - Top vertex: (0.0, 0.5, 0.0)
 * - Bottom-left: (-0.5, -0.5, 0.0)  
 * - Bottom-right: (0.5, -0.5, 0.0)
 */
const float triangleVertices[] = {
    0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
};

/**
 * @brief Triangle vertex colors in RGBA format
 * 
 * Assigns different colors to each vertex for color interpolation:
 * - Top vertex: Yellow (1.0, 1.0, 0.0, 1.0)
 * - Bottom-left: Magenta (0.0, 1.0, 1.0, 1.0)
 * - Bottom-right: Cyan (1.0, 0.0, 1.0, 1.0)
 */
const float triangleColors[] = {
    1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0,
};
