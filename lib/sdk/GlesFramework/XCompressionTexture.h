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
 * @file XCompressionTexture.h
 *
 * This file implements all the functions related to texture compression ETC.
 *
*******************************************************************************/
#ifndef XCOMPRESSIONTEXTURE_H
#define XCOMPRESSIONTEXTURE_H

#include <GLES2/gl2.h>

/**
 * @class ETCHeaderforTexture
 * @brief Represents the header information for an ETC compressed texture.
 * 
 * This class encapsulates the data structure for handling the header of an
 * ETC (Ericsson Texture Compression) compressed texture, including methods
 * to extract the texture's dimensions and padded dimensions.
 */
class ETCHeaderforTexture
{
private:
    unsigned char paddedWidthMSB; ///< Most significant byte of the padded width.
    unsigned char paddedWidthLSB; ///< Least significant byte of the padded width.
    unsigned char paddedHeightMSB; ///< Most significant byte of the padded height.
    unsigned char paddedHeightLSB; ///< Least significant byte of the padded height.
    unsigned char widthMSB; ///< Most significant byte of the width.
    unsigned char widthLSB; ///< Least significant byte of the width.
    unsigned char heightMSB; ///< Most significant byte of the height.
    unsigned char heightLSB; ///< Least significant byte of the height.

public:
    /**
     * @brief Default constructor.
     * 
     * Initializes an ETCHeaderforTexture object with default values.
     */
    ETCHeaderforTexture();

    /**
     * @brief Parameterized constructor.
     * 
     * Initializes an ETCHeaderforTexture object with the provided data.
     * 
     * @param data Pointer to the raw data containing the header information.
     */
    ETCHeaderforTexture(unsigned char *data);

    /**
     * @brief Gets the width of the texture.
     * 
     * @return The width of the texture as an unsigned short.
     */
    unsigned short getWidth(void);

    /**
     * @brief Gets the height of the texture.
     * 
     * @return The height of the texture as an unsigned short.
     */
    unsigned short getHeight(void);

    /**
     * @brief Gets the padded width of the texture.
     * 
     * @return The padded width of the texture as an unsigned short.
     */
    unsigned short getPaddedWidth(void);

    /**
     * @brief Gets the padded height of the texture.
     * 
     * @return The padded height of the texture as an unsigned short.
     */
    unsigned short getPaddedHeight(void);

    /**
     * @brief Calculates the size of the texture.
     * 
     * The size is computed based on the internal format of the texture.
     * 
     * @param internalFormat The internal format of the texture (e.g., GL_ETC1_RGB8_OES).
     * @return The size of the texture in bytes as a GLsizei.
     */
    GLsizei getSize(GLenum internalFormat);
};

#endif /* XCOMPRESSIONTEXTURE_H */
