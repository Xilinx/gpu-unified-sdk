/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/** 
 * 
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 *
 *
 * @file MDRRendererTexture.cpp
 *
 * @brief This class unlike the MDRFBO represents virtual render target based on 2D texture functionality. Instead of using FBO it simply copy framebuffer to texture.
 *
*******************************************************************************/

#include "MDRRendererTexture.h"

using namespace SDKXilinx;

MDRRendererTexture::MDRRendererTexture(
    bool aUseMipmap /*= true*/)
    :
    BaseClass(),
    //
    theTarget(GL_TEXTURE_2D),
    theWidth(0),
    theHeight(0),
    theUseMipmap(aUseMipmap),
    //
    theIndex(0)
  {
  memset(theName, 0, TEXTURES_COUNT * sizeof(GLuint));
  }

MDRRendererTexture::~MDRRendererTexture()
  {
  destroy();
  }

bool MDRRendererTexture::create(
    unsigned int aWidth,
    unsigned int aHeight,
    const unsigned char* aData)
  {
  theWidth = aWidth;
  theHeight = aHeight;

  /* Load just base level texture data. */
  if (theName[theIndex] == 0)
    {
    GL_CHECK(glGenTextures(TEXTURES_COUNT, theName));
    }

  /* Create textures. */
  for (unsigned int index = 0; index < TEXTURES_COUNT; index++)
    {
    GL_CHECK(glBindTexture(theTarget, theName[index]));
    GL_CHECK(glTexImage2D(theTarget, 0, GL_RGBA, theWidth, theHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, aData));

    /* Set texture filtering. */
    if (theUseMipmap)
      {
      GL_CHECK(glTexParameteri(theTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
      }
    else
      {
      GL_CHECK(glTexParameteri(theTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
      }
    GL_CHECK(glTexParameteri(theTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); /* Default anyway. */
    GL_CHECK(glTexParameteri(theTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(theTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    if (theUseMipmap)
      {
      GL_CHECK(glGenerateMipmap(theTarget));
      }
    }

  return true;
  }

bool MDRRendererTexture::initialize(
    unsigned int aWidth,
    unsigned int aHeight)
  {
  return create(aWidth, aHeight, NULL);
  }

bool MDRRendererTexture::bindRT() const
  {
  theIndex = (theIndex + 1) % TEXTURES_COUNT;

  return true;
  }

void MDRRendererTexture::unbindRT() const
  {
  bindTexture();
  GL_CHECK(glCopyTexImage2D(theTarget,
                            0, // level
                            GL_RGBA, // internalformat
                            0, // x
                            0, // y
                            theWidth, // width
                            theHeight, // height
                            0 // border
                            )
           );
  // Generate mipmap
  if (theUseMipmap)
    {
    GL_CHECK(glGenerateMipmap(theTarget));
    }
  }

void MDRRendererTexture::bindTexture() const
  {
  GL_CHECK(glBindTexture(theTarget, theName[theIndex]));
  }

bool MDRRendererTexture::destroy()
  {
  if (theName[theIndex] == 0)
    return true;
  //
  GL_CHECK(glDeleteTextures(TEXTURES_COUNT, theName));
  for (unsigned int index = 0; index < TEXTURES_COUNT; index++)
    theName[index] = 0;
  //
  return true;
  }
