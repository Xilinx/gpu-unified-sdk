// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.

attribute vec4 av4position;
attribute vec3 av3color;

uniform mat4 mvp;

varying vec3 vv3color;

void main()
{
    vv3color = av3color;
    gl_Position =  mvp * av4position;
}
