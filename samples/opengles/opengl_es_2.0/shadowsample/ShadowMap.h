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

#ifndef SHADOWMAP_HPP
#define SHADOWMAP_HPP

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifdef _MSC_VER
#include <time.h>
#include <Winsock2.h>
#else
#include <sys/time.h>
#endif

#define GLES_VERSION 2

#include "XGLSLCompile.h"
#include "XTexture.h"
#include "XMatrixAPI.h"
#include "XTime.h"

/* Windows, GNU Linux (x86 and ARM) */
#include <EGL/egl.h>
#include <assert.h>
  #include "XPodium.h"
  #include "XEGLIntf.h"


#define GL_CHECK(x) \
        x; \
{ \
        GLenum glError = glGetError(); \
        if(glError != GL_NO_ERROR) { \
                printf("glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
                exit(1); \
        } \
}

#define LOGD

#define LOGE

#define LOGI



#define EGL_CHECK(x) \
    x; \
    { \
        EGLint eglError = eglGetError(); \
        if(eglError != EGL_SUCCESS) { \
            LOGE("eglGetError() = %i (0x%.8x) at %s:%i\n", (signed int)eglError, (unsigned int)eglError, __FILE__, __LINE__); \
            exit(1); \
        } \
    }


template <class Type>
int getPtrDiff(Type a1,
               Type a2)
  {
  return (int) (a1 - a2);
  }

template <class Type>
void deleteAndNULL(Type& aValue)
  {
  if (aValue != NULL)
    {
    delete aValue;
    aValue = NULL;
    }
  }

#endif
