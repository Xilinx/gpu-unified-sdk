// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.

precision lowp float;

varying vec3 vv3color;

void main()
{
    gl_FragColor =  vec4(vv3color, 1.0);
}
