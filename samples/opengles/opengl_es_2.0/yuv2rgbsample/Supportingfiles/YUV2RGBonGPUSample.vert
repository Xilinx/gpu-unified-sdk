// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.

precision mediump float;
attribute vec4 av4position;
attribute vec2 av3colour;
uniform mat4 mvp;
varying vec2 textureOut;
void main()
{
  textureOut =  av3colour;
  gl_Position =  mvp * av4position;
}

