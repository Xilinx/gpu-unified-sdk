/* Copyright (c) 2014-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Modifications Copyright (C) 2024-2025 Advanced Micro Devices, Inc.  All rights reserved.
 */

#ifndef __3d_h
#define __3d_h

#include <GLES2/gl2.h>

/* Tri strips, so quads are in this order:
 *
 * 2 ----- 3
 * | \     |
 * |   \   |
 * |     \ |
 * 0 ----- 1
 */
static const float aFloorVertex[] =
{
  -6.0f, -0.5f,  6.0f,
   6.0f, -0.5f,  6.0f,
  -6.0f, -0.5f, -6.0f,
   6.0f, -0.5f, -6.0f,
};

static const float aFloorColor[] =
{
  0.1f, 0.3f,  0.1f,
  0.1f, 0.3f,  0.1f,
  0.1f, 0.3f,  0.1f,
  0.1f, 0.3f,  0.1f,
};

static const float aFloorNormal[] =
{
  0.0f, 1.0f,  0.0f,
  0.0f, 1.0f,  0.0f,
  0.0f, 1.0f,  0.0f,
  0.0f, 1.0f,  0.0f,
};

static const GLushort aFloorIndex[] =
{
  0, 1, 2, 1, 3, 2
};

/* Tri strips, so quads are in this order:
 *
 * 2 ----- 3
 * | \     |
 * |   \   | 4 - 5
 * |     \ | | / |
 * 0 ----- 1 6 - 7
 */
static const float aCubeVertex[] =
{
  //FRONT
  -0.5f, -0.5f,  0.5f, /* 0 */
   0.5f, -0.5f,  0.5f, /* 1 */
  -0.5f,  0.5f,  0.5f, /* 2 */
   0.5f, -0.5f,  0.5f, /* 1 */
   0.5f,  0.5f,  0.5f, /* 3 */
  -0.5f,  0.5f,  0.5f, /* 2 */

  // BACK
  -0.5f, -0.5f, -0.5f, /* 6 */
  -0.5f,  0.5f, -0.5f, /* 4 */
   0.5f,  0.5f, -0.5f, /* 5 */
  -0.5f, -0.5f, -0.5f, /* 6 */
   0.5f,  0.5f, -0.5f, /* 5 */
   0.5f, -0.5f, -0.5f, /* 7 */

  // TOP
  -0.5f,  0.5f,  0.5f, /* 2 */
   0.5f,  0.5f,  0.5f, /* 3 */
   0.5f,  0.5f, -0.5f, /* 5 */
  -0.5f,  0.5f,  0.5f, /* 2 */
   0.5f,  0.5f, -0.5f, /* 5 */
  -0.5f,  0.5f, -0.5f, /* 4 */

  // BOTTOM
  -0.5f, -0.5f,  0.5f, /* 0 */
   0.5f, -0.5f,  0.5f, /* 1 */
   0.5f, -0.5f, -0.5f, /* 7 */
  -0.5f, -0.5f,  0.5f, /* 0 */
   0.5f, -0.5f, -0.5f, /* 7 */
  -0.5f, -0.5f, -0.5f, /* 6 */

  // LEFT
  -0.5f, -0.5f, -0.5f, /* 6 */
  -0.5f, -0.5f,  0.5f, /* 0 */
  -0.5f,  0.5f,  0.5f, /* 2 */
  -0.5f, -0.5f, -0.5f, /* 6 */
  -0.5f,  0.5f,  0.5f, /* 2 */
  -0.5f,  0.5f, -0.5f, /* 4 */

  // RIGHT
   0.5f, -0.5f,  0.5f, /* 1 */
   0.5f, -0.5f, -0.5f, /* 7 */
   0.5f,  0.5f, -0.5f, /* 5 */
   0.5f, -0.5f,  0.5f, /* 1 */
   0.5f,  0.5f, -0.5f, /* 5 */
   0.5f,  0.5f,  0.5f, /* 3 */
};

static const float aCubeNormal[] =
{
  //FRONT
   0.0f,  0.0f,  1.0f, /* 0 */
   0.0f,  0.0f,  1.0f, /* 1 */
   0.0f,  0.0f,  1.0f, /* 2 */
   0.0f,  0.0f,  1.0f, /* 1 */
   0.0f,  0.0f,  1.0f, /* 3 */
   0.0f,  0.0f,  1.0f, /* 2 */

  // BACK
   0.0f,  0.0f, -1.0f, /* 6 */
   0.0f,  0.0f, -1.0f, /* 4 */
   0.0f,  0.0f, -1.0f, /* 5 */
   0.0f,  0.0f, -1.0f, /* 6 */
   0.0f,  0.0f, -1.0f, /* 5 */
   0.0f,  0.0f, -1.0f, /* 7 */

  // TOP
   0.0f,  1.0f,  0.0f, /* 2 */
   0.0f,  1.0f,  0.0f, /* 3 */
   0.0f,  1.0f,  0.0f, /* 5 */
   0.0f,  1.0f,  0.0f, /* 2 */
   0.0f,  1.0f,  0.0f, /* 5 */
   0.0f,  1.0f,  0.0f, /* 4 */

  // BOTTOM
   0.0f, -1.0f,  0.0f, /* 0 */
   0.0f, -1.0f,  0.0f, /* 1 */
   0.0f, -1.0f,  0.0f, /* 7 */
   0.0f, -1.0f,  0.0f, /* 0 */
   0.0f, -1.0f,  0.0f, /* 7 */
   0.0f, -1.0f,  0.0f, /* 6 */

  // LEFT
  -1.0f,  0.0f,  0.0f, /* 6 */
  -1.0f,  0.0f,  0.0f, /* 0 */
  -1.0f,  0.0f,  0.0f, /* 2 */
  -1.0f,  0.0f,  0.0f, /* 6 */
  -1.0f,  0.0f,  0.0f, /* 2 */
  -1.0f,  0.0f,  0.0f, /* 4 */

  // RIGHT
   1.0f,  0.0f,  0.0f, /* 1 */
   1.0f,  0.0f,  0.0f, /* 7 */
   1.0f,  0.0f,  0.0f, /* 5 */
   1.0f,  0.0f,  0.0f, /* 1 */
   1.0f,  0.0f,  0.0f, /* 5 */
   1.0f,  0.0f,  0.0f, /* 3 */
};

static const float aCubeColor[] =
{
  //FRONT
  0.0f, 0.0f, 1.0f, 1.0f, /* 0 */
  1.0f, 0.0f, 1.0f, 1.0f, /* 1 */
  0.0f, 0.0f, 1.0f, 1.0f, /* 2 */
  0.0f, 0.0f, 1.0f, 1.0f, /* 1 */
  0.0f, 0.0f, 1.0f, 1.0f, /* 3 */
  0.0f, 0.0f, 1.0f, 1.0f, /* 2 */

  // BACK
  0.0f, 1.0f, 1.0f, 1.0f, /* 6 */
  0.0f, 0.0f, 1.0f, 1.0f, /* 4 */
  1.0f, 0.0f, 1.0f, 1.0f, /* 5 */
  0.0f, 1.0f, 1.0f, 1.0f, /* 6 */
  1.0f, 0.0f, 1.0f, 1.0f, /* 5 */
  1.0f, 1.0f, 1.0f, 1.0f, /* 7 */

  // TOP
  0.0f, 1.0f, 0.0f, 1.0f, /* 2 */
  1.0f, 1.0f, 0.0f, 1.0f, /* 3 */
  1.0f, 0.0f, 1.0f, 1.0f, /* 5 */
  0.0f, 1.0f, 0.0f, 1.0f, /* 2 */
  1.0f, 0.0f, 1.0f, 1.0f, /* 5 */
  0.0f, 0.0f, 1.0f, 1.0f, /* 4 */

  // BOTTOM
  0.0f, 0.0f, 0.0f, 1.0f, /* 0 */
  1.0f, 0.0f, 0.0f, 1.0f, /* 1 */
  1.0f, 1.0f, 1.0f, 1.0f, /* 7 */
  0.0f, 0.0f, 0.0f, 1.0f, /* 0 */
  1.0f, 1.0f, 1.0f, 1.0f, /* 7 */
  0.0f, 1.0f, 1.0f, 1.0f, /* 6 */

  // LEFT
  0.0f, 1.0f, 1.0f, 1.0f, /* 6 */
  0.0f, 0.0f, 0.0f, 1.0f, /* 0 */
  0.0f, 1.0f, 0.0f, 1.0f, /* 2 */
  0.0f, 1.0f, 1.0f, 1.0f, /* 6 */
  0.0f, 1.0f, 0.0f, 1.0f, /* 2 */
  0.0f, 0.0f, 1.0f, 1.0f, /* 4 */

  // RIGHT
  1.0f, 0.0f, 0.0f, 1.0f, /* 1 */
  1.0f, 1.0f, 1.0f, 1.0f, /* 7 */
  1.0f, 0.0f, 1.0f, 1.0f, /* 5 */
  1.0f, 0.0f, 0.0f, 1.0f, /* 1 */
  1.0f, 0.0f, 1.0f, 1.0f, /* 5 */
  1.0f, 1.0f, 0.0f, 1.0f, /* 3 */
};

static const float aCubeTexCoord[] =
{
  //FRONT
  0.0, 0.0, /* 0 */
  1.0, 0.0, /* 1 */
  0.0, 1.0, /* 2 */
  1.0, 0.0, /* 1 */
  1.0, 1.0, /* 3 */
  0.0, 1.0, /* 2 */

  // BACK
  0.0, 0.0, /* 6 */
  0.0, 1.0, /* 4 */
  1.0, 1.0, /* 5 */
  0.0, 0.0, /* 6 */
  1.0, 1.0, /* 5 */
  1.0, 0.0, /* 7 */

  // TOP
  0.0, 1.0, /* 2 */
  1.0, 1.0, /* 3 */
  1.0, 1.0, /* 5 */
  0.0, 1.0, /* 2 */
  1.0, 1.0, /* 5 */
  0.0, 1.0, /* 4 */

  // BOTTOM
  0.0, 0.0, /* 0 */
  1.0, 0.0, /* 1 */
  1.0, 0.0, /* 7 */
  0.0, 0.0, /* 0 */
  1.0, 0.0, /* 7 */
  0.0, 0.0, /* 6 */

  // LEFT
  0.0, 0.0, /* 6 */
  0.0, 0.0, /* 0 */
  0.0, 1.0, /* 2 */
  0.0, 0.0, /* 6 */
  0.0, 1.0, /* 2 */
  0.0, 1.0, /* 4 */

  // RIGHT
  1.0, 0.0, /* 1 */
  1.0, 0.0, /* 7 */
  1.0, 1.0, /* 5 */
  1.0, 0.0, /* 1 */
  1.0, 1.0, /* 5 */
  1.0, 1.0, /* 3 */
};

#endif /* __3d_h */

