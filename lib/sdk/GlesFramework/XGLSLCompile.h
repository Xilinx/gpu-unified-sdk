/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/**
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
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