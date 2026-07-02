#version 100
/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */

precision mediump float;
varying vec4 v_v4TexCoord;
void main()
{
	/* Generate shadow map - write fragment depth. */
	float value = 10.0 - v_v4TexCoord.z;
	float v = floor(value);
	float f = value - v;
	float vn = v * 0.1;
	gl_FragColor = vec4(vn, f, 0.0, 1.0);	
}

