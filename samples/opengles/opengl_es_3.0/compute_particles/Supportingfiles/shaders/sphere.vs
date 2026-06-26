#version 310 es


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
in vec3 position;

out vec2 vShadowTexel;
out vec3 vNormal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projectionViewLight;

void main()
{
    vec4 mPosition = model * vec4(position, 1.0);
    vNormal = (model * vec4(position, 0.0)).xyz;
    gl_Position = projection * view * mPosition;

    // project onto shadowmap
    vec4 posFromLight = projectionViewLight * mPosition;
    vShadowTexel = posFromLight.xy * 0.5 + vec2(0.5);
}
