/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
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
