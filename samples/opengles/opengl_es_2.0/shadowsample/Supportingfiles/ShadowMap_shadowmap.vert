/* vim:set sts=4 ts=4 noexpandtab cindent syntax=c: */
/*
 *
 * Copyright (c) _2024, Advanced Micro Devices, Inc.
 * All rights reserved.
 * Developed by:
 *           AMD Software Development
 *           Advanced Micro Devices, Inc.
 *           www.amd.com

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to
 * deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimers in
 *   the documentation and/or other materials provided with the distribution.
 * - Neither the names of <Name of Development Group, Name of Institution>,
 *   nor the names of its contributors may be used to endorse or promote
 *   products derived from this Software without specific prior written
 *   permission.
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 */

#version 100

attribute vec4 a_v4Position;
attribute vec4 a_v4Normal;
attribute vec4 a_v4FillColor;

uniform mat4 u_m4MVP;
uniform mat4 u_m4M;
uniform mat4 u_m4MV;
uniform mat4 u_m4InvTransMV;
uniform mat4 u_m4LightV;
uniform mat4 u_m4LightP;
uniform vec4 u_v4LightPos;

varying vec4 v_v4FillColor;
varying vec4 v_v4Normal;
varying vec4 v_v4TexCoord;
varying vec4 v_v4LightDir;
varying vec4 v_v4ViewDir;

void main()
{
	const mat4 biasMat = mat4(0.5, 0.0, 0.0, 0.0,
	                          0.0, 0.5, 0.0, 0.0,
	                          0.0, 0.0, 1.0, 0.0,
	                          0.5, 0.5, 0.0, 1.0);

	/* Calculate Light dir for shading geometry */
	vec3 vPos = vec3(u_m4MV * a_v4Position);
	vec3 lPos = u_v4LightPos.xyz;
	v_v4LightDir = vec4(lPos - vPos, 1.0);
	
	/* Calculate ViewDir. Though it is better practice to calculate the vector in the application and pass it to the shader. */
	v_v4ViewDir = -vec4(u_m4MV * vec4(0.0, 0.0, 0.0, 1.0));
		
	/* Calculate vertex position, which is being seen from the light. */
	v_v4TexCoord =  u_m4LightP * u_m4LightV * u_m4M * a_v4Position;

	/* Normalize texture coords from -1..1 to 0..1 now, before projection. */
	v_v4TexCoord =  biasMat * v_v4TexCoord;
	
	// Calculate normals they should be multiplied by Transposed Inverse Model View matrix
	v_v4Normal = normalize(u_m4InvTransMV * vec4(a_v4Normal.xyz, 1.0));
	v_v4FillColor = a_v4FillColor;

	gl_Position = u_m4MVP * a_v4Position;
}

