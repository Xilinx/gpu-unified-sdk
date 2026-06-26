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

#include "FontEngine.h"
#include "XTexture.h"
#include "XGLSLCompile.h"
#include "XPodium.h"
#include "XCVector.h"

#include <cstring>

#define GL_CHECK(x) \
        x; \
{ \
        GLenum glError = glGetError(); \
        if(glError != GL_NO_ERROR) { \
                printf("glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
                exit(1); \
        } \
}


using std::string;

/**
 * @namespace SDKXilinx
 * @brief AMD GPU SDK namespace containing graphics and computation utilities.
 * 
 * This namespace contains various classes and utilities for GPU programming,
 * including font rendering, texture management, matrix operations, and EGL
 * interface management for AMD GPU SDK applications.
 */
namespace SDKXilinx
{ 
    // Static constant definitions for FontEngine class
    const string FontEngine::textureFilename = "font.raw";
    const string FontEngine::vertexShaderFilename = "font.vert";
    const string FontEngine::fragmentShaderFilename = "font.frag";

    const float FontEngine::scale = 1.0f;

    const int FontEngine::textureCharacterWidth = 8;
    const int FontEngine::textureCharacterHeight = 16;

    /**
     * @brief Constructor for FontEngine.
     * 
     * Initializes the FontEngine with the specified resource directory and window dimensions.
     * Sets up shaders, creates the orthographic projection matrix, loads the font texture,
     * and initializes all necessary OpenGL resources for text rendering.
     * 
     * @param resourceDirectory Path to the directory containing shader and texture resources
     * @param windowWidth Width of the target window in pixels
     * @param windowHeight Height of the target window in pixels
     * 
     * @throws Exits with error code 1 if shader attribute locations cannot be found
     */
    FontEngine::FontEngine(const char * resourceDirectory, int windowWidth, int windowHeight)
    {
        vertexShaderID = 0;
        fragmentShaderID = 0;
        programID = 0;
        
        numberOfCharacters = 0;
        textVertex = NULL;
        textTextureCoordinates = NULL;
        color = NULL;
        textIndex = NULL;


        /* Create an orthographic projection. */
        projectionMatrix = XMatrixAPI::XMatrixAPIOrthographic(0, (float)windowWidth, 0, (float)windowHeight, 0, 1);

        /* Shaders. */
        string vertexShader = resourceDirectory + vertexShaderFilename;
        Shader::processShader(&vertexShaderID, vertexShader.c_str(), GL_VERTEX_SHADER);
        
        string fragmentShader = resourceDirectory + fragmentShaderFilename;
        Shader::processShader(&fragmentShaderID, fragmentShader.c_str(), GL_FRAGMENT_SHADER);

        /* Set up shaders. */
        programID = GL_CHECK(glCreateProgram());
        GL_CHECK(glAttachShader(programID, vertexShaderID));
        GL_CHECK(glAttachShader(programID, fragmentShaderID));
        GL_CHECK(glLinkProgram(programID));
        GL_CHECK(glUseProgram(programID));

        /* Vertex positions. */
        m_iLocPosition = GL_CHECK(glGetAttribLocation(programID, "a_v4Position"));
        if(m_iLocPosition == -1)
        {
            exit(1);
        }

        /* FontEngine colors. */
        m_iLocTextColor = GL_CHECK(glGetAttribLocation(programID, "a_v4FontColor"));
        if(m_iLocTextColor == -1)
        {
            exit(1);
        }

        /* TexCoords. */
        m_iLocTexCoord = GL_CHECK(glGetAttribLocation(programID, "a_v2TexCoord"));
        if(m_iLocTexCoord == -1)
        {
            exit(1);
        }

        /* Projection matrix. */
        m_iLocProjection = GL_CHECK(glGetUniformLocation(programID, "u_m4Projection"));
        if(m_iLocProjection == -1)
        {
        }
        else
        {
            GL_CHECK(glUniformMatrix4fv(m_iLocProjection, 1, GL_FALSE, projectionMatrix.getAsArray()));
        }


        /* Set the sampler to point at the 0th texture unit. */
        m_iLocTexture = GL_CHECK(glGetUniformLocation(programID, "u_s2dTexture"));
        if(m_iLocTexture == -1)
        {
        }
        else
        {
            GL_CHECK(glUniform1i(m_iLocTexture, 0));
        }

        /* Load texture. */
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glGenTextures(1, &textureID));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
        /* Set filtering. */
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        string texture = resourceDirectory + textureFilename;
        unsigned char *textureData = NULL;
        XTexture::XloadData(texture.c_str(), &textureData);

        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 48, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData));
        free(textureData);
        textureData = NULL;

    }

    /**
     * @brief Clears all current text data and frees associated memory.
     * 
     * This method resets the numberOfCharacters counter to zero and deallocates
     * all dynamically allocated memory for vertex data, texture coordinates,
     * color data, and index arrays. Should be called before adding new text
     * or when cleaning up the FontEngine.
     * 
     * @note All pointers are set to NULL after freeing to prevent dangling references
     */
    void FontEngine::clear(void)
    {
        numberOfCharacters = 0;

        free(textVertex);
        free(textTextureCoordinates);
        free(color);
        free(textIndex);

        textVertex = NULL;
        textTextureCoordinates = NULL;
        color = NULL;
        textIndex = NULL;
    }

    /**
     * @brief Adds a string to be rendered with specified position and color.
     * 
     * This method processes the input string character by character, calculates the
     * appropriate texture coordinates for each character from the font texture atlas,
     * generates vertex positions, and stores color information. The method handles
     * dynamic memory allocation to accommodate additional characters and rebuilds
     * the index array for triangle strip rendering.
     * 
     * @param xPosition X coordinate for the start of the text (pixels from left edge)
     * @param yPosition Y coordinate for the start of the text (pixels from bottom edge)
     * @param string Null-terminated C-style string to render
     * @param red Red color component (0-255)
     * @param green Green color component (0-255)
     * @param blue Blue color component (0-255)
     * @param alpha Alpha (transparency) component (0-255, where 0 is transparent)
     * 
     * @note This method reallocates memory for vertex arrays to accommodate new text.
     *       The texture coordinates are calculated based on a 32x3 character grid in the font texture.
     * @note Characters are mapped starting from ASCII 32 (space character).
     * 
     * @throws Exits with error code 1 if memory allocation fails
     */
    void FontEngine::addString(int xPosition, int yPosition, const char *string, int red, int green, int blue, int alpha)
    {
        int length = (int)strlen(string);
        int iTexCoordPos = 4 * 2 * numberOfCharacters;
        int iVertexPos = 4 * 3 * numberOfCharacters;
        int iColorPos = 4 * 4 * numberOfCharacters;
        int iIndex = 0;
        int iIndexPos = 0;

        numberOfCharacters += length;

        /* Realloc memory. */
        textVertex = (float *)realloc(textVertex, numberOfCharacters * 4 * 3 * sizeof(float));
        textTextureCoordinates = (float *)realloc(textTextureCoordinates, numberOfCharacters * 4 * 2 * sizeof(float));
        color = (float *)realloc(color, numberOfCharacters * 4 * 4 * sizeof(float));
        textIndex = (GLshort *)realloc(textIndex, (numberOfCharacters * 6 - 2) * sizeof(GLshort));
        if((textVertex == NULL) || (textTextureCoordinates == NULL) || (color == NULL) || (textIndex == NULL))
        {
            exit(1);
        }

        /* Re-init entire index array. */
        textIndex[iIndex ++] = 0;
        textIndex[iIndex ++] = 1;
        textIndex[iIndex ++] = 2;
        textIndex[iIndex ++] = 3;

        iIndexPos = 4;
        for(int cIndex = 1; cIndex < numberOfCharacters; cIndex ++)
        {
            textIndex[iIndexPos ++] = iIndex - 1;
            textIndex[iIndexPos ++] = iIndex;
            textIndex[iIndexPos ++] = iIndex++;
            textIndex[iIndexPos ++] = iIndex++;
            textIndex[iIndexPos ++] = iIndex++;
            textIndex[iIndexPos ++] = iIndex++;
        }

        for(int iChar = 0; iChar < (signed int)strlen(string); iChar ++)
        {
            char cChar = string[iChar];
            int iCharX = 0;
            int iCharY = 0;
            CVec2 sBottom_left;
            CVec2 sBottom_right;
            CVec2 sTop_left;
            CVec2 sTop_right;

            /* Calculate tex coord for char here. */
            cChar -= 32;
            iCharX = cChar % 32;
            iCharY = cChar / 32;
            iCharX *= textureCharacterWidth;
            iCharY *= textureCharacterHeight;
            sBottom_left.x = iCharX;
            sBottom_left.y = iCharY;
            sBottom_right.x = iCharX + textureCharacterWidth;
            sBottom_right.y = iCharY;
            sTop_left.x = iCharX;
            sTop_left.y = iCharY + textureCharacterHeight;
            sTop_right.x = iCharX + textureCharacterWidth;
            sTop_right.y = iCharY + textureCharacterHeight;

            /* Add vertex position data here. */
            textVertex[iVertexPos++] = xPosition + iChar * textureCharacterWidth * scale;
            textVertex[iVertexPos++] = (float)yPosition;
            textVertex[iVertexPos++] = 0;

            textVertex[iVertexPos++] = xPosition + (iChar + 1) * textureCharacterWidth * scale;
            textVertex[iVertexPos++] = (float)yPosition;
            textVertex[iVertexPos++] = 0;

            textVertex[iVertexPos++] = xPosition + iChar * textureCharacterWidth * scale;
            textVertex[iVertexPos++] = yPosition + textureCharacterHeight * scale;
            textVertex[iVertexPos++] = 0;

            textVertex[iVertexPos++] = xPosition + (iChar + 1) * textureCharacterWidth * scale;
            textVertex[iVertexPos++] = yPosition + textureCharacterHeight * scale;
            textVertex[iVertexPos++] = 0;

            /* TextureEngine coords here. Because textures are read in upside down, flip Y coords here. */
            textTextureCoordinates[iTexCoordPos++] = sBottom_left.x / 256.0f;
            textTextureCoordinates[iTexCoordPos++] = sTop_left.y / 48.0f;

            textTextureCoordinates[iTexCoordPos++] = sBottom_right.x / 256.0f;
            textTextureCoordinates[iTexCoordPos++] = sTop_right.y / 48.0f;

            textTextureCoordinates[iTexCoordPos++] = sTop_left.x / 256.0f;
            textTextureCoordinates[iTexCoordPos++] = sBottom_left.y / 48.0f;

            textTextureCoordinates[iTexCoordPos++] = sTop_right.x / 256.0f;
            textTextureCoordinates[iTexCoordPos++] = sBottom_right.y / 48.0f;

            /* Color data. */
            color[iColorPos ++] = red / 255.0f;
            color[iColorPos ++] = green / 255.0f;
            color[iColorPos ++] = blue / 255.0f;
            color[iColorPos ++] = alpha / 255.0f;

            /* Copy to the other 3 vertices. */
            memcpy(&color[iColorPos], &color[iColorPos - 4], 4 * sizeof(float));
            memcpy(&color[iColorPos + 4], &color[iColorPos], 4 * sizeof(float));
            memcpy(&color[iColorPos + 8], &color[iColorPos + 4], 4 * sizeof(float));
            iColorPos += 3 * 4;
        }
    }

    /**
     * @brief Renders all added text strings to the current OpenGL context.
     * 
     * This method activates the font shader program, binds the font texture,
     * sets up vertex attribute arrays for position, color, and texture coordinates,
     * and renders all text using a triangle strip with indexed drawing.
     * The method handles OpenGL ES 3.0 compatibility by unbinding vertex arrays
     * and buffers when necessary.
     * 
     * @note If no characters have been added (numberOfCharacters == 0), the method returns early.
     * @note The method assumes that the OpenGL context is properly set up and current.
     * @note Vertex attribute arrays are enabled before drawing and disabled afterwards.
     * 
     * @pre The FontEngine must be properly initialized with valid shaders and texture.
     * @post All vertex attribute arrays are disabled after rendering.
     */
    void FontEngine::draw(void)
    {
#if GLES_VERSION == 3
        GL_CHECK(glBindVertexArray(0));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
#endif
        if(numberOfCharacters == 0) 
        {
            return;
        }

        GL_CHECK(glUseProgram(programID));

        if(m_iLocPosition != -1)
        {
            GL_CHECK(glEnableVertexAttribArray(m_iLocPosition));
            GL_CHECK(glVertexAttribPointer(m_iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, textVertex));
        }

        if(m_iLocTextColor != -1)
        {
            GL_CHECK(glEnableVertexAttribArray(m_iLocTextColor));
            GL_CHECK(glVertexAttribPointer(m_iLocTextColor, 4, GL_FLOAT, GL_FALSE, 0, color));
        }

        if(m_iLocTexCoord != -1)
        {
            GL_CHECK(glEnableVertexAttribArray(m_iLocTexCoord));
            GL_CHECK(glVertexAttribPointer(m_iLocTexCoord, 2, GL_FLOAT, GL_FALSE, 0, textTextureCoordinates));
        }

        if(m_iLocProjection != -1)
        {
            GL_CHECK(glUniformMatrix4fv(m_iLocProjection, 1, GL_FALSE, projectionMatrix.getAsArray()));
        }

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));

        GL_CHECK(glDrawElements(GL_TRIANGLE_STRIP, numberOfCharacters * 6 - 2, GL_UNSIGNED_SHORT, textIndex));

        if(m_iLocTextColor != -1)
        {
            GL_CHECK(glDisableVertexAttribArray(m_iLocTextColor));
        }

        if(m_iLocTexCoord != -1)
        {
            GL_CHECK(glDisableVertexAttribArray(m_iLocTexCoord));
        }

        if(m_iLocPosition != -1)
        {
            GL_CHECK(glDisableVertexAttribArray(m_iLocPosition));
        }
    }

    /**
     * @brief Destructor for FontEngine.
     * 
     * Cleans up all allocated resources including dynamically allocated memory
     * for text data and OpenGL resources such as the font texture. Calls the
     * clear() method to free vertex arrays and then deletes the OpenGL texture object.
     * 
     * @note This destructor ensures proper cleanup of both CPU memory and GPU resources.
     */
    FontEngine::~FontEngine(void)
    {
        clear();
        
        GL_CHECK(glDeleteTextures(1, &textureID));
    }
}
