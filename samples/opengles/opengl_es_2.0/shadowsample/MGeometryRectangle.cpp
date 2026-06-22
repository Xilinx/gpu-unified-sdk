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
 * @file MGeometryRectangle.cpp
 *
 * @brief This class is inherited from a MGeometryBase class and represents a rectangle geometry.
 *
*******************************************************************************/

#include "MGeometryRectangle.h"

MGeometryRectangle::MGeometryRectangle()
    :
    BaseClass()
  {
  }

MGeometryRectangle::~MGeometryRectangle()
  {
  }


void MGeometryRectangle::set(
    float aWidth,
    float aHeight,
    unsigned int aHorizontal,
    unsigned int aVertical)
  {
  MRendererPrimitive& primitive = getPrimitive();
  //
  primitive.clearAll();
  primitive.setMode(MRendererPrimitive::MODE_TRIANGLE_STRIP);
  primitive.setDrawingMode(MRendererPrimitive::DRAWING_ELEMENTS);
  //
  MVector3f point(0.0f, 0.0f, 0.0f);
  MVector3f normal(0.0f, 0.0f, 1.0f);
  MVector3f coord(0.0f, 0.0f, 0.0f);
  float hW = aWidth / 2.0f;
  float hH = aHeight / 2.0f;
  unsigned int index = 0;
  for (unsigned int ix = 0; ix <= aHorizontal; ix++)
    {
    point[0] = hW - (float)ix * (aWidth / (float)aHorizontal);
    coord[0] = (float)ix / (float)aHorizontal;
    for (unsigned int iy = 0; iy <= aVertical; iy++)
      {
      point[1] = hH - (float)iy * (aHeight / (float)aVertical);
      coord[1] = (float)iy / (float)aVertical;
      //
      primitive.append(MRendererPrimitive::ATTRIB_VERTICES, point);
      primitive.append(MRendererPrimitive::ATTRIB_NORMALS, normal);
      primitive.append(MRendererPrimitive::ATTRIB_COLORS, point + 0.5f);
      primitive.append(MRendererPrimitive::ATTRIB_TEXCOORDS_0, coord);

      primitive.appendIndex(index++);
      }
    }
  }
