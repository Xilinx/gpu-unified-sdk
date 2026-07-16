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
 * @file XCompressionTexture.cpp
 *
 * This file implements all the functions related to texture compression ETC.
 *
*******************************************************************************/

#include "XCompressionTexture.h"


    unsigned short ETCHeaderforTexture::getWidth(void)
    {
        return (widthMSB << 8) | widthLSB;
    }

    unsigned short ETCHeaderforTexture::getHeight(void)
    {
        return (heightMSB << 8) | heightLSB;
    }

    unsigned short ETCHeaderforTexture::getPaddedWidth(void)
    {
        return (paddedWidthMSB << 8) | paddedWidthLSB;
    }

    unsigned short ETCHeaderforTexture::getPaddedHeight(void)
    {
        return (paddedHeightMSB << 8) | paddedHeightLSB;
    }

    GLsizei ETCHeaderforTexture::getSize(GLenum internalFormat)
    {
        return (getPaddedWidth() * getPaddedHeight());
    }


    ETCHeaderforTexture::ETCHeaderforTexture()
    {
    
    }

    ETCHeaderforTexture::ETCHeaderforTexture(unsigned char *data)
    {
  
        paddedWidthMSB = data[8];
        paddedWidthLSB = data[9];
        paddedHeightMSB = data[10];
        paddedHeightLSB = data[11];
        widthMSB = data[12];
        widthLSB = data[13];
        heightMSB = data[14];
        heightLSB = data[15];
    }
