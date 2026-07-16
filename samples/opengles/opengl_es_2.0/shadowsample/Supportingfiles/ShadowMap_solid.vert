#version 100
/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */

attribute vec4 a_v4Position;

uniform mat4 u_m4MVP;
uniform vec4 u_v4FillColor;

varying vec4 v_v4FillColor;

void main()
  {
	v_v4FillColor = u_v4FillColor;
	gl_Position = u_m4MVP * a_v4Position;
  }

