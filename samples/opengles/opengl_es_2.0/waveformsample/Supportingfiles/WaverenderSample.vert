// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.

attribute vec4 a_v4Position;
uniform mat4 mvp;


void main()
{
    gl_Position =  mvp * a_v4Position;
}
