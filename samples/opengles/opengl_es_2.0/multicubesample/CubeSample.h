/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/******************************************************************************/
/**
 *
 * @file BasicCube.h
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

/** @brief Display width in pixels for the rendering window */

#ifndef WINDOW_W
#define WINDOW_W 1920
#endif


/** @brief Display height in pixels for the rendering window */
#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

/** @brief Directory path for font shader resources */
string fontResourceDirectory = "FontShaders/";

/** @brief Directory path for sample-specific supporting files */
string resourceDirectory = "Supportingfiles/";

/** @brief Filename for the vertex shader source code */
string vertexShaderFilename = "CubeSample.vert";

/** @brief Filename for the fragment shader source code */
string fragmentShaderFilename = "CubeSample.frag";

/** @brief OpenGL program ID for the compiled shader program */
GLuint programID;

/** @brief Attribute location for vertex position data in shader */
GLint iLocPosition = -1;

/** @brief Attribute location for vertex color data in shader */
GLint iLocFillColor = -1;

/** @brief Uniform location for model-view-projection matrix in shader */
GLint iLocMVP = -1;

/**
 * @brief Vertex data for cube geometry
 * 
 * Contains 3D coordinates for all vertices of a cube.
 * Array contains 108 floats (36 vertices * 3 coordinates each).
 * Vertices are organized as triangles for OpenGL rendering.
 * Each face of the cube consists of 2 triangles (6 vertices).
 */
const float cubeVertices[] = {

    -0.7, 0.7,  -0.7, 0.7,  -0.7, -0.7, -0.7, -0.7, -0.7, -0.7, 0.7,  -0.7, 0.7,  0.7,  -0.7, 0.7,  -0.7, -0.7,
    -0.7, 0.7,  0.7,  -0.7, -0.7, -0.7, -0.7, -0.7, 0.7,  -0.7, 0.7,  0.7,  -0.7, 0.7,  -0.7, -0.7, -0.7, -0.7,
    -0.7, 0.7,  0.7,  0.7,  0.7,  -0.7, -0.7, 0.7,  -0.7, -0.7, 0.7,  0.7,  0.7,  0.7,  0.7,  0.7,  0.7,  -0.7,
    0.7,  0.7,  -0.7, 0.7,  -0.7, 0.7,  0.7,  -0.7, -0.7, 0.7,  0.7,  -0.7, 0.7,  0.7,  0.7,  0.7,  -0.7, 0.7,
    0.7,  0.7,  0.7,  -0.7, -0.7, 0.7,  0.7,  -0.7, 0.7,  0.7,  0.7,  0.7,  -0.7, 0.7,  0.7,  -0.7, -0.7, 0.7,
    -0.7, -0.7, -0.7, 0.7,  -0.7, 0.7,  -0.7, -0.7, 0.7,  -0.7, -0.7, -0.7, 0.7,  -0.7, -0.7, 0.7,  -0.7, 0.7,

};

/**
 * @brief Color data for cube vertices
 * 
 * Contains RGB color values for each vertex of the cube.
 * Array contains 108 floats (36 vertices * 3 color components each).
 * Colors are organized to match the vertex order in cubeVertices array.
 * Each vertex gets its own color creating a colorful gradient effect on the cube faces.
 * Color values are in RGB format with range 0.0 to 1.0.
 */
const float cubeColors[] = {
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 1.0, /* blue */
    1.0, 0.0, 0.0, /* green */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* yellow */
    1.0, 0.0, 1.0, /* blue */
    1.0, 1.0, 0.0, /* white */
    1.0, 1.0, 0.0, /* green */
    1.0, 1.0, 0.0, /* cyan */
    1.0, 1.0, 0.0, /* white */
    1.0, 1.0, 0.0, /* red */
    1.0, 1.0, 0.0, /* green */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 0.0, 0.0, /* red */
    1.0, 1.0, 1.0, /* white */
    0.0, 0.0, 0.0, /* black */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 0.0, 1.0, /* blue */
    1.0, 1.0, 0.0, /* yellow */
    0.0, 0.0, 0.0, /* black */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 0.0, 0.0, /* black */
    0.0, 1.0, 1.0, /* cyan */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 0.0, 0.0, /* black */
    1.0, 1.0, 1.0, /* white */
    0.0, 1.0, 1.0, /* cyan */
    0.0, 1.0, 0.0, /* green */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 1.0, 1.0, /* cyan */
    0.0, 1.0, 0.0, /* green */
    0.0, 0.0, 1.0, /* blue */
    1.0, 0.0, 1.0, /* magenta */

};
