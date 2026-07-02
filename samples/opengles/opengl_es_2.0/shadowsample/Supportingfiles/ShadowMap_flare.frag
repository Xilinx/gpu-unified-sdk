#version 100
/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */

precision mediump float;

uniform sampler2D u_s2dTexture;

varying vec4 v_v4FillColor;
varying vec3 v_v3TexCoord;

void main()
  {
	gl_FragColor = v_v4FillColor;
	gl_FragColor = texture2D(u_s2dTexture, v_v3TexCoord.xy) * v_v4FillColor;
  }

