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
