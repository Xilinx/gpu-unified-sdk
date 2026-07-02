/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/******************************************************************************/
/**
 *
 * @file Depth.h
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

#ifndef FETCH_DEPTH_H
#define FETCH_DEPTH_H

#define GLES_VERSION 2
#include <GLES2/gl2.h>
#include <string>


using namespace std;

string resourceDirectory              = "Supportingfiles/";
string vertexShaderCubeFilename       = "DepthSamplebox.vert";
string fragmentShaderCubeFilename     = "DepthSamplebox.frag";
string vertexShaderGlassFilename      = "DepthSamplevertical.vert";
string fragmentShaderGlassFilename    = "DepthSamplevertical.frag";
string fragmentShaderGlassRttFilename = "DepthSampleverticalExt.frag";

GLuint programCubeID;
GLint iLocCubePosition;
GLint iLocCubeColor;
GLint iLocCubeMVP;

GLuint programGlassID;
GLint iLocGlassPosition;
GLint iLocGlassColor;
GLint iLocGlassMVP;
GLint iLocNear;
GLint iLocFar;
GLint iLocGlassMaterial;

int windowWidth = -1;
int windowHeight = -1;


const float colorsCube[] =
{
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */
    1.0, 0.0, 0.0, /* red */

    0.0, 1.0, 1.0, /* blue */
    0.0, 1.0, 1.0, /* blue */
    0.0, 1.0, 1.0, /* blue */
    0.0, 0.0, 1.0, /* blue */
    0.0, 0.0, 1.0, /* blue */
    0.0, 0.0, 1.0, /* blue */

    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 1.0, /* white */

    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 1.0, 0.0, /* yellow */

    1.0, 0.0, 1.0, /* cyan */
    1.0, 0.0, 1.0, /* cyan */
    1.0, 0.0, 1.0, /* cyan */
    0.0, 1.0, 1.0, /* cyan */
    0.0, 1.0, 1.0, /* cyan */
    0.0, 1.0, 1.0, /* cyan */

    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
    1.0, 0.2, 1.0, /* magenta */
};


const float verticesGlass[] =
{
    0.3, -0.5,  0,
    0.3,  0.8,  0,
    -0.3, -0.5,  0,
    -0.3,  0.8,  0,
};

const float verticesCube[] =
{
    -0.5,  0.5, -0.5,
    0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5,  0.5, -0.5,
    0.5,  0.5, -0.5,
    0.5, -0.5, -0.5,
    -0.5,  0.5,  0.5,
    -0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,
    -0.5,  0.5,  0.5,
    -0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5,  0.5,  0.5,
    0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
    -0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,
    0.5,  0.5, -0.5,
    0.5,  0.5, -0.5,
    0.5, -0.5,  0.5,
    0.5, -0.5, -0.5,
    0.5,  0.5, -0.5,
    0.5,  0.5,  0.5,
    0.5, -0.5,  0.5,
    0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
    0.5, -0.5,  0.5,
    0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
    -0.5, -0.5, -0.5,
    0.5, -0.5,  0.5,
    -0.5, -0.5,  0.5,
    -0.5, -0.5, -0.5,
    0.5, -0.5, -0.5,
    0.5, -0.5,  0.5,
};

const float colorsGlass[] =
{
    0.2, 1.0, 0.0, 0.2, /* green */
    0.2, 1.0, 0.0, 0.2, /* green */
    1.0, 0.0, 0.0, 0.2, /* green */
    1.0, 0.0, 0.0, 0.2, /* green */
};



#endif /* FETCH_DEPTH_H */
