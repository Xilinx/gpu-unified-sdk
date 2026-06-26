/* vim:set sts=4 ts=4 noexpandtab cindent syntax=c: */
/* Copyright (c) 2014-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Modifications Copyright (C) 2024-2025 Advanced Micro Devices, Inc.  All rights reserved.
 */

#version 100

precision mediump float;

uniform sampler2D u_s2dShadowMap;
uniform vec4 u_v3Specular;
uniform vec4 u_v3GlobalAmbient;

varying vec4 v_v4TexCoord;
varying vec4 v_v4Normal;
varying vec4 v_v4FillColor;
varying vec4 v_v4LightDir;
varying vec4 v_v4ViewDir;

void main()
{
	const vec3 Ambient = vec3(0.1, 0.1, 0.1);
	const float Specular = 90.0;
	vec3 LightDir = normalize( v_v4LightDir.xyz );
	vec3 Half = normalize( v_v4LightDir.xyz + v_v4ViewDir.xyz );
	vec3 Normal = normalize( v_v4Normal.xyz );

	/* Calculate diffuse intensity */
	float NoL = dot( Normal, LightDir );
	float intensityDiffuse = clamp(NoL, 0.0, 1.0);
	
	/* Calculate specular intensity */
	float NoH = clamp(dot(Normal, Half), 0.0, 1.0);
	float intensitySpecular = pow(NoH, Specular);

	/* Draw main scene - read and compare shadow map. */
	vec2 vfDepth = texture2DProj(u_s2dShadowMap, v_v4TexCoord).xy;
	float fDepth = (vfDepth.x * 10.0 + vfDepth.y);
	
	/* Unpack the light distance. See how it is packed in the shadow.frag file */
	float fLDepth = (10.0-v_v4TexCoord.z) + 0.1 - fDepth ;
	float fLight = 1.0;
	if(fDepth>0.0 && fLDepth<0.0)
		{
		fLight = 0.2;
		/* Make sure there is no specular effect on obscured fragments */
		intensitySpecular = 0.0;
		}
	
	gl_FragColor = u_v3GlobalAmbient + vec4(Ambient * v_v4FillColor.rgb +
                                          v_v4FillColor.rgb * fLight * intensityDiffuse +
                                          u_v3Specular.rgb * intensitySpecular,
                                          1.0);
}

