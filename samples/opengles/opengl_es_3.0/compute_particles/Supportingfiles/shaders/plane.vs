#version 310 es

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
in vec3 position;
in vec3 normal;

out vec3 vPosition;
out vec2 vShadowTexel;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projectionViewLight;

void main()
{
    vPosition = (model * vec4(position, 1.0)).xyz;
    gl_Position = projection * view * vec4(vPosition, 1.0);

    // project onto shadowmap
    vec4 posFromLight = projectionViewLight * vec4(vPosition, 1.0);
    vShadowTexel = posFromLight.xy * 0.5 + vec2(0.5);
}
