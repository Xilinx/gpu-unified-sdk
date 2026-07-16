#version 100
/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */

attribute vec4 a_v4Position;
uniform mat4 u_m4LightMVP;
varying vec4 v_v4TexCoord;

void main()
{
	v_v4TexCoord = u_m4LightMVP * a_v4Position;
	gl_Position = u_m4LightMVP * a_v4Position;
}

