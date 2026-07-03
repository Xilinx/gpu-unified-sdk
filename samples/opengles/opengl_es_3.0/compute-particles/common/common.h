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
 * Modifications Copyright (C) 2024-2026 Advanced Micro Devices, Inc.  All rights reserved.
 */

#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdlib>
#include <GLES3/gl31.h>
//#include "EGLRuntime.h"
#include <EGL/egl.h>
#include <assert.h>
#include "XPodium.h"
#include "XEGLIntf.h"


#include <string>
#include <vector>
#include <stdint.h>
#include "matrix.h"


using std::string;
using namespace std;

using std::string;
using std::vector;

#define LOGD

#define LOGE

#define LOGI

#define ASSERT(x, s)                                                \
if (!(x))                                                           \
{                                                                   \
    LOGE("Assertion failed at %s:%i\n%s\n", __FILE__, __LINE__, s); \
    exit(1);                                                        \
}

#endif /* COMMON_H */
