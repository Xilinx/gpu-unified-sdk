#version 100
/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */

precision mediump float;

varying vec4 v_v4FillColor;

void main()
  {
	gl_FragColor = v_v4FillColor;
  }

