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
 * @file MDRRendererTarget.cpp
 *
 * @brief This class represents two types of render targets such as: Frame Buffer Object and TextureEngine. When the FBO mode is used all graphics go to the Frame Buffer Object,
 * whereas the Copy TextureEngine mode is used all graphics is first rendered to the framebuffer and then copied to the texture.
 *
*******************************************************************************/
#include "MDRRenderTarget.h"
#include "MDRRendererTexture.h"

using namespace SDKXilinx;

MDRRenderTarget::MDRRenderTarget(
    MDRMode aMode)
    :
    BaseClass(),
    //
    theWidth(0),
    theHeight(0),
    theRenderable(NULL)
  {
    theRenderable = new MDRRendererTexture(false);
  }

MDRRenderTarget::~MDRRenderTarget()
  {
  deleteAndNULL(theRenderable);
  }

bool MDRRenderTarget::initialize(
    unsigned int aWidth,
    unsigned int aHeight)
  {
  theWidth = aWidth;
  theHeight = aHeight;
  return theRenderable->initialize(theWidth, theHeight);
  }


bool MDRRenderTarget::bindRT() const
  {
  bool result = theRenderable->bindRT();
    /* Draw into FBO. */
  GL_CHECK(glViewport(0, 0, theWidth, theHeight));

  return result;
  }

void MDRRenderTarget::unbindRT() const
  {
  theRenderable->unbindRT();
  }

void MDRRenderTarget::bindTexture() const
  {
  theRenderable->bindTexture();
  }

bool MDRRenderTarget::destroy()
  {
  return theRenderable->destroy();
  }
