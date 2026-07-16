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
 * Modifications Copyright (C) 2024-2026 Advanced Micro Devices, Inc.  All rights reserved.
 */

#ifndef TEXT_H
#define TEXT_H

#include "XMatrixAPI.h"

#include <GLES2/gl2.h>

#include <string>

namespace SDKAMD
{
    /**
     * @class FontEngine
     * @brief Efficient text rendering engine for OpenGL ES applications.
     *
     * The FontEngine class provides a complete solution for rendering text in OpenGL ES
     * using texture-based bitmap fonts. It uses a single texture atlas containing ASCII
     * characters and renders text using OpenGL triangle strips for optimal performance.
     * 
     * Key features:
     * - Texture-based font rendering using a bitmap font atlas
     * - Support for multiple text strings with individual colors
     * - Efficient rendering using vertex arrays and index buffers
     * - Orthographic projection for 2D text positioning
     * - Dynamic memory management for text data
     * 
     * Usage example:
     * @code
     * FontEngine font("/path/to/resources/", 800, 600);
     * font.addString(10, 10, "Hello World!", 255, 255, 255, 255);
     * font.draw();
     * @endcode
     * 
     * @note The font texture must be a 256x48 pixel RGBA texture with characters
     *       arranged in a 32x3 grid, starting from ASCII character 32 (space).
     * @note Coordinates use a bottom-left origin coordinate system.
     */
    class FontEngine
    {
    private:
        // Static resource filenames
        static const std::string textureFilename;      ///< Font texture filename ("font.raw")
        static const std::string vertexShaderFilename; ///< Vertex shader filename ("font.vert")
        static const std::string fragmentShaderFilename; ///< Fragment shader filename ("font.frag")
        
        /**
         * @brief Scaling factor for rendered text.
         * 
         * Controls the size of rendered characters relative to their texture size.
         * A value of 1.0f renders characters at their original texture resolution.
         * 
         * @warning Experimental feature. Values outside 0.75-3.0 may cause rendering issues.
         */
        static const float scale;
        
        // OpenGL rendering state
        XMatrixAPI projectionMatrix;    ///< Orthographic projection matrix for 2D rendering
        
        // Text rendering data (dynamically allocated)
        int numberOfCharacters;         ///< Total number of characters currently stored
        float *textVertex;              ///< Vertex position data (3 floats per vertex, 4 vertices per character)
        float *textTextureCoordinates;  ///< Texture coordinate data (2 floats per vertex, 4 vertices per character)
        float *color;                   ///< Color data (4 floats per vertex RGBA, 4 vertices per character)
        GLshort *textIndex;             ///< Index array for triangle strip rendering
        
        // OpenGL shader and attribute locations
        int m_iLocPosition;             ///< Vertex position attribute location
        int m_iLocProjection;           ///< Projection matrix uniform location
        int m_iLocTextColor;            ///< Text color attribute location
        int m_iLocTexCoord;             ///< Texture coordinate attribute location
        int m_iLocTexture;              ///< Texture sampler uniform location
        
        // OpenGL resource handles
        GLuint vertexShaderID;          ///< Compiled vertex shader object ID
        GLuint fragmentShaderID;        ///< Compiled fragment shader object ID
        GLuint programID;               ///< Linked shader program object ID
        GLuint textureID;               ///< Font texture object ID

    public: 
        /**
         * @brief Width of each character in the font texture (in pixels).
         * 
         * This constant defines the width of each character cell in the font texture atlas.
         * The default value assumes an 8-pixel wide character grid.
         * 
         * @warning Only modify this value if using a different font texture with different character dimensions.
         */
        static const int textureCharacterWidth;
        
        /**
         * @brief Height of each character in the font texture (in pixels).
         * 
         * This constant defines the height of each character cell in the font texture atlas.
         * The default value assumes a 16-pixel tall character grid.
         * 
         * @warning Only modify this value if using a different font texture with different character dimensions.
         */
        static const int textureCharacterHeight;
        
        /**
         * @brief Constructs a FontEngine instance.
         * 
         * Initializes the FontEngine with the specified resource directory and window dimensions.
         * This constructor:
         * - Creates an orthographic projection matrix
         * - Loads and compiles vertex and fragment shaders
         * - Sets up the shader program and retrieves attribute/uniform locations
         * - Loads the font texture and configures texture parameters
         * - Initializes all rendering state variables
         * 
         * @param resourceDirectory Path to the directory containing font.raw, font.vert, and font.frag files
         * @param windowWidth Width of the target rendering window in pixels
         * @param windowHeight Height of the target rendering window in pixels
         * 
         * @throws std::runtime_error Implicitly exits if shader attributes cannot be located or resources fail to load
         * 
         * @note The resource directory path should end with a path separator
         * @note Window dimensions are used to create the orthographic projection matrix
         */
        FontEngine(const char * resourceDirectory, int windowWidth, int windowHeight);
        
        /**
         * @brief Destroys the FontEngine instance.
         * 
         * Cleans up all allocated resources including:
         * - Dynamically allocated vertex, texture coordinate, color, and index arrays
         * - OpenGL texture object
         * 
         * @note Shader objects and program are not explicitly deleted as they may be managed by OpenGL
         */
        virtual ~FontEngine(void);
        
        /**
         * @brief Clears all current text data.
         *
         * Removes all previously added text strings and frees associated memory.
         * This method should be called before adding new text for each frame to
         * avoid accumulating text from previous frames.
         * 
         * @note After calling this method, numberOfCharacters is reset to 0
         * @note All dynamic arrays are freed and set to NULL
         */
        void clear(void);

        /**
         * @brief Adds a text string to be rendered.
         * 
         * Processes the input string and generates the necessary vertex data, texture coordinates,
         * and color information for rendering. Characters are positioned sequentially starting
         * from the specified position. The method handles dynamic memory allocation to store
         * the additional character data.
         * 
         * @param xPosition X coordinate for the left edge of the first character (pixels from left)
         * @param yPosition Y coordinate for the bottom edge of the text (pixels from bottom)
         * @param string Null-terminated C-style string to render (supports ASCII 32-127)
         * @param red Red color component (0-255, where 255 is full red)
         * @param green Green color component (0-255, where 255 is full green)
         * @param blue Blue color component (0-255, where 255 is full blue)
         * @param alpha Alpha (transparency) component (0-255, where 255 is opaque, 0 is transparent)
         * 
         * @note Multiple calls to this method accumulate text for batch rendering
         * @note Character positions are calculated automatically based on textureCharacterWidth
         * @note The method rebuilds the entire index array to maintain triangle strip connectivity
         * 
         * @warning The method exits with error code 1 if memory allocation fails
         */
        void addString(int xPosition, int yPosition, const char *string, int red, int green, int blue, int alpha);

        /**
         * @brief Renders all added text to the current OpenGL context.
         * 
         * Draws all text strings that have been added since the last clear() call.
         * The method:
         * - Activates the font shader program
         * - Binds vertex attribute arrays for position, color, and texture coordinates
         * - Sets the projection matrix uniform
         * - Binds the font texture
         * - Renders using triangle strips with indexed drawing
         * - Cleans up by disabling vertex attribute arrays
         * 
         * @pre OpenGL context must be current and properly initialized
         * @pre FontEngine must be successfully constructed with valid shaders and texture
         * 
         * @post All vertex attribute arrays are disabled
         * 
         * @note If no characters have been added, the method returns immediately without rendering
         * @note Compatible with both OpenGL ES 2.0 and 3.0
         */
        void draw(void);
    };
}
#endif /* TEXT_H */
