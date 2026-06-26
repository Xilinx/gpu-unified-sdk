#version 310 es


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
precision mediump float;

in vec4 mask0;

out vec4 out0;

void main()
{
    vec2 xy = 2.0 * gl_PointCoord.xy - vec2(1.0);
    float r2 = dot(xy, xy);
    float opacity = exp2(-r2 * 5.0) * 0.025;
    out0 = opacity * mask0;
}
