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
 * 
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 * @endcond
 *
 *
 * @file MDRRenderer.cpp
 *
 * @brief This class represents an abstract object, which could be used as a render target (e.g. FBO).
 *
*******************************************************************************/

#include "MDRRenderer.h"

using namespace SDKXilinx;

MDRRenderer::MDRRenderer()
    :
    theWidth(0xFFFFFFFF),
    theHeight(0xFFFFFFFF),
    //
    theMatrixP(),
    //
    theFrameCounter(0)
  {
  }

MDRRenderer::~MDRRenderer()
  {
  }

bool MDRRenderer::initialize(
    unsigned int aWidth,
    unsigned int aHeight)
  {
  theWidth = aWidth;
  theHeight = aHeight;

  // Calculate projective matrix
  theMatrixP.setPerspective(45.0f, (float)theWidth/(float)theHeight, 0.1f, 100.0f);

  /* Initialize OpenGL-ES. */
  GL_CHECK(glClearColor(0.2f, 0.2f, 0.2f, 0.0f));
  GL_CHECK(glClearDepthf(0.0f));
  GL_CHECK(glEnable(GL_BLEND));
  GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GL_CHECK(glDisable(GL_CULL_FACE));
  GL_CHECK(glEnable(GL_DEPTH_TEST));
  GL_CHECK(glDepthMask(false));
  GL_CHECK(glDepthFunc(GL_GREATER));

  return true;
  }

void MDRRenderer::preFrameRender()
  {
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
  }

void MDRRenderer::postFrameRender()
  {
  theFrameCounter++;
  }

void MDRRenderer::activateTexture(
    MDRTexUnit aUnit,
    const MDRRendererTexture& aTexture)
  {
  GL_CHECK(glActiveTexture(GL_TEXTURE0 + aUnit));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, aTexture.getName()));
  }
