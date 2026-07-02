/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/******************************************************************************/
/**
 *
 * @file yuv2rgbonGPU.h
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

static const GLfloat vertexVertices[] = {

        -1.0f, -1.0f,

        1.0f, -1.0f,

        -1.0f,  1.0f,

        1.0f,  1.0f,

};

static const GLfloat textureVertices[] = {

        0.0f,  1.0f,

        1.0f,  1.0f,

        0.0f,  0.0f,

        1.0f,  0.0f,

};


