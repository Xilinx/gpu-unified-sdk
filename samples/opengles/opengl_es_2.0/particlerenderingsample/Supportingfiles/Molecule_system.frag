// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.

precision mediump float;

varying float v_ageFactor;
varying vec3 v_v3colour;
uniform sampler2D s_texture;

void main()
{
    vec4 texColour;
    vec2 texCoords;
    float alphaFactor;

    texCoords = vec2(gl_PointCoord.x, gl_PointCoord.y);

    /* Define an alpha modulation factor depending of the particle's age. */
    if(v_ageFactor  <= 0.5)
    {
        alphaFactor = 0.14 * v_ageFactor;
    }
    else
    {
        alphaFactor = -0.14 * v_ageFactor + 0.14;
    }

    vec4 baseColour = vec4(v_v3colour.x, v_v3colour.y, v_v3colour.z, 1.0);
    texColour =  texture2D(s_texture, texCoords);

    /* Modulate alpha component. */
    texColour.a = texColour.a * alphaFactor;

    gl_FragColor = vec4(baseColour.rgb * texColour.r, texColour.a);
}
