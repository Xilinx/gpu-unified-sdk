/*******************************************************************************
* @cond CC
 * Copyright (C) 2024 AMD, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a AMD device, or
 * (b) that interact with a AMD device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AMD CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the AMD shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from AMD.
 *
*******************************************************************************/
/**
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 * @endcond
 *
*******************************************************************************/
/**
 *
 * @file XGLSLCompile.h
 *
 * @brief This file implements all the functions required to compile GLSL shader code. 
 *
*******************************************************************************/

#ifndef XGLSLCOMPILE_H
#define XGLSLCOMPILE_H

#include <GLES2/gl2.h>

/**
 * @class Shader
 * @brief A class for handling shader operations, including loading and processing shaders.
 * 
 * This class provides static methods to load shaders from files and process them
 * for use with OpenGL ES 2.0. It is designed to work with GLSL (OpenGL Shading Language)
 * shaders.
 */
class Shader
{
private:
    /**
     * @brief Loads the shader source code from a file.
     * 
     * This private static method reads the content of a shader file into a dynamically
     * allocated string. The file is expected to contain the source code of the shader.
     * 
     * @param filename The path to the shader file.
     * @return A pointer to a dynamically allocated string containing the shader source code.
     */
    static char *loadShader(const char *filename);

public:
    /**
     * @brief Processes a shader by compiling it from a source file.
     * 
     * This static method creates, compiles, and attaches a shader to an OpenGL ES program.
     * It reads the shader source code from the specified file, compiles it, and checks for
     * compilation errors.
     * 
     * @param shader A pointer to a GLuint variable where the compiled shader ID will be stored.
     * @param filename The path to the shader source file.
     * @param shaderType The type of the shader, either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
     */
    static void processShader(GLuint *shader, const char *filename, GLint shaderType);
};

#endif /* XGLSLCOMPILE_H */