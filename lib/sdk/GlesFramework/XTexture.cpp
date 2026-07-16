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
 * @file Texture.cpp
 * @brief This file implements all the functions related to texture compression techniques.
 *
*******************************************************************************/



#include "XTexture.h"
#include "XCompressionTexture.h"
#include "XPodium.h"

#include <GLES2/gl2ext.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using std::string;



    /**
     * @brief Loads compressed texture data from a PKM file.
     * 
     * This method loads texture data from a PKM (PowerVR Texture) file format and
     * extracts the ETC (Ericsson Texture Compression) header information. The PKM
     * format includes a 16-byte header followed by the compressed texture data.
     * 
     * @param filename Path to the PKM file to load
     * @param etcHeader Pointer to ETCHeaderforTexture object to store header information
     * @param textureData Pointer to pointer where the loaded texture data will be stored
     * 
     * @throws Exits with error code 1 if textureData or etcHeader parameters are NULL
     */
    void XTexture::XloadPKMData(const char *filename, ETCHeaderforTexture* etcHeader, unsigned char **textureData)
    {
        const int       sizeOfETCHeaderforTexture = 16; 
        unsigned char*  tempTextureData = NULL;

        XloadData(filename, &tempTextureData);

        if (textureData == NULL)
        {
            exit(1);
        }
        if (etcHeader == NULL)
        {
            exit(1);
        }

        if (tempTextureData != NULL)
        {
            ETCHeaderforTexture tempEtcHeader(tempTextureData);

            *etcHeader   = tempEtcHeader;
            *textureData = tempTextureData + sizeOfETCHeaderforTexture;
        }
        else
        {
            exit(1);
        }
    }

    /**
     * @brief Loads compressed mipmaps from a series of files.
     * 
     * This method generates a complete mipmap chain by loading compressed texture files
     * with sequential numbering. It automatically calculates the number of mipmap levels
     * based on the base texture dimensions and loads each level from separate files.
     * 
     * File naming convention: filenameBase + level + filenameSuffix
     * Example: "texture0.pkm", "texture1.pkm", "texture2.pkm", etc.
     * 
     * @param filenameBase Base part of the filename (e.g., "texture")
     * @param filenameSuffix File extension/suffix (e.g., ".pkm")
     * @param textureID Pointer to GLuint where the generated texture ID will be stored
     * 
     * @note The method assumes ETC1 compression format (GL_ETC1_RGB8_OES)
     * @note Automatically calculates and loads the complete mipmap chain
     */
    void XTexture::XloadCompressedMipmaps(const char *filenameBase, const char *filenameSuffix, GLuint *textureID)
    {
        glGenTextures(1, textureID);
        glBindTexture(GL_TEXTURE_2D, *textureID);
        
        string filename = filenameBase + string("0") + filenameSuffix;
        unsigned char *data = NULL;
        XloadData(filename.c_str(), &data);
        ETCHeaderforTexture loadedETCHeaderforTexture = ETCHeaderforTexture(data);

        int width = loadedETCHeaderforTexture.getWidth();
        int height = loadedETCHeaderforTexture.getHeight();
        int numberOfMipmaps = 1;
        while((width > 1) || (height > 1))
        {
            numberOfMipmaps ++;
            if(width > 1) width >>= 1;
            if(height > 1) height >>= 1;
        }
    
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, loadedETCHeaderforTexture.getWidth(), loadedETCHeaderforTexture.getHeight(), 0, (loadedETCHeaderforTexture.getPaddedWidth() * loadedETCHeaderforTexture.getPaddedHeight()) >> 1, data + 16);
        free(data);
        data = NULL;

        for(int allMipmaps = 1; allMipmaps < numberOfMipmaps; allMipmaps++)
        {
            char *level = NULL;
            int input = allMipmaps;
            int inputLength = 0;

            do
            {
                input /= 10;
                inputLength ++;
            }
            while(input != 0);

            level = (char *)calloc(inputLength + 1, sizeof(char));
            if(level == NULL)
            {
                exit(1);
            }
            sprintf(level, "%i", allMipmaps);

            filename = filenameBase + string(level) + filenameSuffix;
            XloadData(filename.c_str(), &data);
            free(level);
            level = NULL;
            loadedETCHeaderforTexture = ETCHeaderforTexture(data);

            glCompressedTexImage2D(GL_TEXTURE_2D, allMipmaps, GL_ETC1_RGB8_OES, loadedETCHeaderforTexture.getWidth(), loadedETCHeaderforTexture.getHeight(), 0, (loadedETCHeaderforTexture.getPaddedWidth() * loadedETCHeaderforTexture.getPaddedHeight()) >> 1, data + 16);
            free(data);
            data = NULL;
        }
    }

    /**
     * @brief Retrieves the compressed texture formats supported by the implementation.
     * 
     * This private method queries the OpenGL ES implementation for the list of
     * supported compressed texture formats. The formats are retrieved using
     * glGetIntegerv with GL_COMPRESSED_TEXTURE_FORMATS.
     * 
     * @param textureFormats Pointer to GLint pointer where the array of formats will be allocated and stored
     * @param numberOfTextureFormats Pointer to integer where the count of supported formats will be stored
     * 
     * @note The caller is responsible for freeing the allocated textureFormats array
     * @throws Exits with error code 1 if memory allocation fails
     */
    void XTexture::XgetCompressedTextureFormats(GLint** textureFormats, int* numberOfTextureFormats)
    {
        glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, numberOfTextureFormats);

        *textureFormats = (GLint *)calloc(*numberOfTextureFormats, sizeof(GLint));
        if(*textureFormats == NULL)
        {
            exit(1);
        }

        glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, *textureFormats);
    }

    /**
     * @brief Checks if ETC1 texture compression is supported.
     * 
     * This method queries the OpenGL ES implementation to determine if ETC1 (Ericsson Texture
     * Compression) format is supported. ETC1 is a lossy texture compression format commonly
     * supported on mobile GPU implementations.
     * 
     * @param verbose If true, additional debug information may be printed to stdout
     * @return True if ETC1 compression (GL_ETC1_RGB8_OES) is supported, false otherwise
     */
    bool XTexture::XisETCSupported(bool verbose)
    {
        bool supportETC = false;
        GLint *textureFormat = NULL;
        GLint numberOfTextureFormats = 0;
        XgetCompressedTextureFormats(&textureFormat, &numberOfTextureFormats);

        if (verbose != 0)
        {
            for(int allTextureFormats = 0; allTextureFormats < numberOfTextureFormats; allTextureFormats++)
            {
                switch(textureFormat[allTextureFormats])
                {
                    case GL_ETC1_RGB8_OES:
                        break;
                    default:
                        break;
                }
            }
        }  

        for(int allTextureFormats = 0; allTextureFormats < numberOfTextureFormats; allTextureFormats++)
        {
            if (textureFormat[allTextureFormats] == GL_ETC1_RGB8_OES)
            {
                supportETC = true;
            }
        }

        free(textureFormat);
        textureFormat = NULL;

        if(!supportETC)
        {
            printf("BAD ETC\n");
        }
        return supportETC;
    }

    void XTexture::XcreateTexture(unsigned int width, unsigned int height, GLvoid **textureData)
    {
        unsigned char *randomTexture = new unsigned char [width * height * 4];
        if(randomTexture == NULL)
        {
            exit(1);
        }

        for(unsigned int allTexels = 0; allTexels < width * height; allTexels ++)
        {
            randomTexture[allTexels * 4 + 3] = 255;
            for (int allChannels = 0; allChannels < 3; allChannels++)
            {
                int randomNumber = (int)(255 * (rand() / (float)RAND_MAX));
                randomTexture[allTexels * 4 + allChannels] = randomNumber;
            }
        }

        *textureData = randomTexture;
    }

    void XTexture::XcreateTexture(unsigned int width, unsigned int height, unsigned int red, GLvoid **textureData)
    {
        unsigned char* newTexture = new unsigned char [width * height];

        if(newTexture == NULL)
        {
            exit(1);
        }

        for (unsigned int texelIndex = 0; texelIndex < width * height; ++texelIndex)
        {
            newTexture[texelIndex] = red;
        }
        *textureData = newTexture;
    }

    void XTexture::XcreateTexture(unsigned int width, unsigned int height, short red, short **textureData)
    {
        *textureData = new short [width * height];

        if (*textureData == NULL)
        {
            exit(1);
        }

        for (unsigned int texelIndex = 0; texelIndex < width * height; ++texelIndex)
        {
            (*textureData)[texelIndex] = red;
        }
    }

    void XTexture::XdeleteTextureData(GLvoid** textureData)
    {
        delete[] (unsigned char*)*textureData;
    }

    void XTexture::XloadData(const char *filename, unsigned char **textureData)
    {
    
        FILE *file = fopen(filename, "rb");
        if(file == NULL)
        {
            exit(1);
        }
        fseek(file, 0, SEEK_END);
        unsigned int length = ftell(file);
        unsigned char *loadedTexture = (unsigned char *)calloc(length, sizeof(unsigned char));
        if(loadedTexture == NULL)
        {
            exit(1);
        }
        fseek(file, 0, SEEK_SET);
        size_t read = fread(loadedTexture, sizeof(unsigned char), length, file);
        if(read != length)
        {
            exit(1);
        }
        fclose(file);

        *textureData = loadedTexture;
    }

    void XTexture::XreversePixelLine(float* destination, const float* source, int lineWidth)
    {
        const int rgbComponentsCount = 3;

        for (int pixelIndex = 0; pixelIndex < lineWidth; ++pixelIndex)
        {
            memcpy(destination + pixelIndex * rgbComponentsCount,
                   source + (lineWidth - pixelIndex - 1) * rgbComponentsCount, 
                   rgbComponentsCount * sizeof(float));
        }
    }
