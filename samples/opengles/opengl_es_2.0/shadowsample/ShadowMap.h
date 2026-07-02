/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
