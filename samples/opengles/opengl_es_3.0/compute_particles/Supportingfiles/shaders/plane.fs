#version 310 es

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
precision mediump float;

in vec3 vPosition;
in vec2 vShadowTexel;

out vec4 outColor;

uniform vec3 color;
uniform sampler2D shadowMap0;

float sampleShadow()
{
    vec4 shadow0 = texture(shadowMap0, vShadowTexel);
    return clamp(dot(shadow0, vec4(1.0)), 0.0, 1.0);
}

void main()
{
    float r2 = dot(vPosition, vPosition);
    outColor.rgb = color * exp2(-r2 * 0.2 + 1.5);
    outColor.a = 1.0;

    // shadow from particles
    float shadow = sampleShadow();
    outColor.rgb = mix(outColor.rgb, color * vec3(0.1, 0.12, 0.15), shadow);

    // approximate gamma correction
    outColor.rgb = sqrt(outColor.rgb);
}
