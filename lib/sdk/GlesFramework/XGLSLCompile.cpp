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
 * @file XGLSLCompile.cpp
 *
 * @brief This file implements all the functions required to compile GLSL shader code. 
 *
*******************************************************************************/

#include "XGLSLCompile.h"
#include "XPodium.h"

#include <cstdio>
#include <cstdlib>



    char* Shader::loadShader(const char *filename)
    {
        FILE *file = fopen(filename, "rb");
        if(file == NULL)
        {
            printf("Cannot read file '%s'\n", filename);
            exit(1);
        }
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET); 
        char *shader = (char *)calloc(length + 1, sizeof(char));
        if(shader == NULL)
        {
            printf("Out of memory at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }
        size_t numberOfBytesRead = fread(shader, sizeof(char), length, file);
        if (numberOfBytesRead != length) 
        {
	    printf("Error reading %s (read %zu of %ld)", filename, numberOfBytesRead, length);
            exit(1);
        }
        shader[length] = '\0';
        fclose(file);

        return shader;
    }

    void Shader::processShader(GLuint *shader, const char *filename, GLint shaderType)
    {  
        const char *strings[1] = { NULL };

        *shader = glCreateShader(shaderType);
        strings[0] = loadShader(filename);
        glShaderSource(*shader, 1, strings, NULL);

        free((void *)(strings[0]));
        strings[0] = NULL;

        glCompileShader(*shader);
        GLint status;
        glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

        if(status != GL_TRUE) 
        {
            GLint length;
            char *debugSource = NULL;
            char *errorLog = NULL;

            glGetShaderiv(*shader, GL_SHADER_SOURCE_LENGTH, &length);
            debugSource = (char *)malloc(length);
            glGetShaderSource(*shader, length, NULL, debugSource);
            printf("Debug source START:\n%s\nDebug source END\n\n", debugSource);
            free(debugSource);

           glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &length);
            errorLog = (char *)malloc(length);
            glGetShaderInfoLog(*shader, length, NULL, errorLog);
            printf("Log START:\n%s\nLog END\n\n", errorLog);
            free(errorLog);

            printf("Compilation FAILED!\n\n");
            exit(1);
        }
    }
