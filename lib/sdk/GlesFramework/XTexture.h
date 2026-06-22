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
 * @file Texture.h
 * @brief This file implements all the functions related to texture compression techniques.
 *
*******************************************************************************/

#ifndef XTEXTURE_H
#define XTEXTURE_H

#include "XCompressionTexture.h"
#include <GLES2/gl2.h>

/**
 * @class XTexture
 * @brief A utility class for managing textures in OpenGL ES.
 * 
 * This class provides static methods for creating, managing, and manipulating
 * textures in OpenGL ES. It includes functionalities for checking ETC support,
 * creating textures from various data formats, loading texture data from files,
 * and handling texture mipmaps.
 */
class XTexture
{
private:
    /**
     * @brief Retrieves the compressed texture formats supported by the implementation.
     * 
     * This method is used internally to populate the list of supported texture formats.
     * 
     * @param textureFormats Pointer to an array of GLint pointers where the formats will be stored.
     * @param numberOfTextureFormats Pointer to an integer where the number of formats will be stored.
     */
    static void XgetCompressedTextureFormats(GLint **textureFormats, int* numberOfTextureFormats);

public:
    /**
     * @brief Checks if ETC (Ericsson Texture Compression) is supported.
     * 
     * This method determines whether the ETC compression format is supported by the current
     * OpenGL ES implementation.
     * 
     * @param verbose If true, additional information may be printed (default is false).
     * @return True if ETC is supported, false otherwise.
     */
    static bool XisETCSupported(bool verbose = false);

    /**
     * @brief Creates a texture from raw data with the specified dimensions.
     * 
     * This method creates a texture with the given width and height using the provided
     * texture data.
     * 
     * @param width The width of the texture.
     * @param height The height of the texture.
     * @param textureData Pointer to the texture data to be used for the texture creation.
     */
    static void XcreateTexture(unsigned int width, unsigned int height, GLvoid **textureData);

    /**
     * @brief Creates a texture with the specified dimensions and a single color.
     * 
     * This method creates a texture with the given width, height, and color value. The entire
     * texture is filled with the specified color.
     * 
     * @param width The width of the texture.
     * @param height The height of the texture.
     * @param red The red color value.
     * @param textureData Pointer to the texture data to be used for the texture creation.
     */
    static void XcreateTexture(unsigned int width, unsigned int height, unsigned int red, GLvoid **textureData);

    /**
     * @brief Creates a texture with the specified dimensions and a single color value.
     * 
     * This method creates a texture with the given width, height, and color value, where the
     * color is represented as a short integer. The entire texture is filled with the specified color.
     * 
     * @param width The width of the texture.
     * @param height The height of the texture.
     * @param red The red color value (short integer).
     * @param textureData Pointer to the texture data to be used for the texture creation.
     */
    static void XcreateTexture(unsigned int width, unsigned int height, short red, short **textureData);

    /**
     * @brief Deletes texture data from memory.
     * 
     * This method frees the memory allocated for texture data.
     * 
     * @param textureData Pointer to the texture data to be deleted.
     */
    static void XdeleteTextureData(GLvoid **textureData);

    /**
     * @brief Loads texture data from a file.
     * 
     * This method loads texture data from a file and stores it in memory.
     * 
     * @param filename The name of the file from which to load the texture data.
     * @param textureData Pointer to a pointer where the loaded texture data will be stored.
     */
    static void XloadData(const char *filename, unsigned char **textureData);

    /**
     * @brief Loads compressed texture data from a PKM file.
     * 
     * This method loads compressed texture data from a PKM (Packed KTX) file and retrieves the
     * ETC header information.
     * 
     * @param filename The name of the PKM file from which to load the texture data.
     * @param etcHeader Pointer to an ETCHeaderforTexture object where the ETC header will be stored.
     * @param textureData Pointer to a pointer where the loaded texture data will be stored.
     */
    static void XloadPKMData(const char *filename, ETCHeaderforTexture* etcHeader, unsigned char **textureData);

    /**
     * @brief Loads compressed mipmaps from files.
     * 
     * This method loads texture mipmaps from a series of files with the specified base name and suffix.
     * 
     * @param filenameBase The base name of the files containing the mipmaps.
     * @param filenameSuffix The suffix to be appended to the base name to form each file name.
     * @param textureID Pointer to a GLuint where the texture ID will be stored.
     */
    static void XloadCompressedMipmaps(const char *filenameBase, const char *filenameSuffix, GLuint *textureID);

    /**
     * @brief Reverses the pixel line from source to destination.
     * 
     * This method reverses the order of pixels in a line and copies the result to the destination.
     * 
     * @param destination Pointer to the array where the reversed pixel line will be stored.
     * @param source Pointer to the array containing the original pixel line.
     * @param lineWidth The width of the line in pixels.
     */
    static void XreversePixelLine(float* destination, const float* source, int lineWidth);
};

#endif /* XTEXTURE_H */