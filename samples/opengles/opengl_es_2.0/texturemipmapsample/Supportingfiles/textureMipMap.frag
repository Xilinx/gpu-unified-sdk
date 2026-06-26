#version 100
// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
precision mediump float;

uniform sampler2D u_s2dTexture;

varying vec2 v_v2TexCoord;

void main()
{
    gl_FragColor =  texture2D(u_s2dTexture, v_v2TexCoord);
}
