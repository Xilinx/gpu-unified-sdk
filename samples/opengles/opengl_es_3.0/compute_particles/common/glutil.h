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

#ifndef UTIL_H
#define UTIL_H

#include "matrix.h"
#include "common.h"
#include "shader.h"
#include <string>
#include <sstream>

/*
    Triangles are either drawn in a clockwise or counterclockwise order. Facets that face away from the viewer
    can be hidden by setting the rasterizer state to cull such facets.
*/
void cull(bool enabled, GLenum front = GL_CCW, GLenum mode = GL_BACK);

/*
    The incoming pixel depth value can be compared with the depth value present in the depth buffer,
    to determine whether the pixel shall be drawn or not.
*/
void depth_test(bool enabled, GLenum func = GL_LEQUAL);

/*
    The incoming pixel can write to the depth buffer, combined with the depth_test function.
*/
void depth_write(bool enabled);

/*
    Pixels can be drawn using a function that blends the incoming (source) RGBA values with the RGBA 
    values that are already in the frame buffer (the destination values).
*/
void blend_mode(bool enabled, GLenum src = GL_ONE, GLenum dest = GL_ONE, GLenum func = GL_FUNC_ADD);

void use_shader(ShaderEngine shader);
void attribfv(string name, GLsizei num_components, GLsizei stride, GLsizei offset);
void unset_attrib(string name);

void uniform(string name, const mat4 &v);
void uniform(string name, const vec4 &v);
void uniform(string name, const vec3 &v);
void uniform(string name, const vec2 &v);
void uniform(string name, double v);
void uniform(string name, float v);
void uniform(string name, int v);
void uniform(string name, unsigned int v);

bool read_file(const std::string &path, std::string &dest);
GLuint gen_buffer(GLenum target, GLsizei size, const void *data);
GLuint gen_buffer(GLenum target, GLenum usage, GLsizei size, const void *data);
void del_buffer(GLuint buffer);

#endif
