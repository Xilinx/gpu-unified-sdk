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
 * @file MGeometryTorus.cpp
 *
 * @brief This class is inherited from a MGeometryBase class and represents a torus geometry.
 *
*******************************************************************************/
#include "MGeometryTorus.h"

MGeometryTorus::MGeometryTorus() : BaseClass(), thePoints(), theNormals() {}

MGeometryTorus::~MGeometryTorus() {}

void MGeometryTorus::set(
    float aRadiusOrbit,
    float aRadiusRing,
    unsigned int aHorizontal,
    unsigned int aVertical) {
  thePoints.clear();
  theNormals.clear();

  MVector3f tmp(0.0f, 0.0f, 0.0f);
  MVector3f ringOrigin(0.0f, 0.0f, 0.0f);

  for (unsigned int iy = 0; iy < aVertical; iy++) {
    float alpha = iy * (360.f / aVertical);
    float rad = alpha * 0.01745329251994329576923690768489;
    float fy = aRadiusRing * cosf(rad);
    float fz = aRadiusOrbit + aRadiusRing * sinf(rad);

    for (unsigned int ix = 0; ix < aHorizontal; ix++) {
      float beta = ix * (360.f / aHorizontal);
      float rad = beta * 0.01745329251994329576923690768489;

      tmp[0] = fz * sinf(rad);
      tmp[1] = fy;
      tmp[2] = fz * cosf(rad);

      ringOrigin[0] = aRadiusOrbit * sinf(rad);
      ringOrigin[2] = aRadiusOrbit * cosf(rad);

      thePoints.append(tmp);

      tmp -= ringOrigin;
      theNormals.append(tmp);
    }
  }

  buildPrimitveTSI(aHorizontal, aVertical, thePoints, theNormals, getPrimitive());
}

void MGeometryTorus::buildPrimitveTSI(
    unsigned int aHorizontal,
    unsigned int aVertical,
    const MArrayVec3f& aPoints,
    const MArrayVec3f& aNormals,
    MRendererPrimitive& aOutPrimitive) const {
  aOutPrimitive.clearAll();
  aOutPrimitive.setMode(MRendererPrimitive::MODE_TRIANGLE_STRIP);
  aOutPrimitive.setDrawingMode(MRendererPrimitive::DRAWING_ELEMENTS);

  unsigned int count = aPoints.getCount();
  for (unsigned int index = 0; index < count; index++) {
    const MVector3f& point = aPoints[index];
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, point);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, aNormals[index]);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(point[0] + 0.6f, point[1] + 0.6f, point[2] + 0.0f));
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_TEXCOORDS_0, point);
  }

  for (unsigned int iy = 1; iy < aVertical; iy++) {
    for (unsigned int ix = 1; ix < aHorizontal; ix++) {
      unsigned int iPrev = (iy - 1) * aHorizontal + (ix - 1);
      unsigned int iCurr = iy * aHorizontal + ix;
      aOutPrimitive.appendIndex(iPrev);
      aOutPrimitive.appendIndex(iCurr);
    }

    unsigned int iLast = (iy - 1) * aHorizontal + (aHorizontal - 1);
    unsigned int iFirstCurr = iy * aHorizontal + 0;
    unsigned int iFirstPrev = (iy - 1) * aHorizontal + 0;
    unsigned int iSecondCurr = iy * aHorizontal + 1;

    aOutPrimitive.appendIndex(iLast);
    aOutPrimitive.appendIndex(iFirstCurr);
    aOutPrimitive.appendIndex(iFirstPrev);
    aOutPrimitive.appendIndex(iSecondCurr);

    aOutPrimitive.appendIndex(iSecondCurr);
    aOutPrimitive.appendIndex(iFirstCurr);
    aOutPrimitive.appendIndex(iFirstCurr);
    aOutPrimitive.appendIndex(iFirstCurr);

    if (iy == (aVertical - 1)) {
      for (unsigned int ix = 1; ix < aHorizontal; ix++) {
        unsigned int iPrev = iy * aHorizontal + (ix - 1);
        unsigned int iCurr = 0 * aHorizontal + ix;
        aOutPrimitive.appendIndex(iPrev);
        aOutPrimitive.appendIndex(iCurr);
      }

      unsigned int iLast = iy * aHorizontal + (aHorizontal - 1);
      unsigned int iFirstCurr = 0 * aHorizontal + 0;
      unsigned int iFirstPrev = iy * aHorizontal + 0;
      unsigned int iSecondCurr = 0 * aHorizontal + 1;

      aOutPrimitive.appendIndex(iLast);
      aOutPrimitive.appendIndex(iFirstCurr);
      aOutPrimitive.appendIndex(iFirstPrev);
      aOutPrimitive.appendIndex(iSecondCurr);
    }
  }
}

void MGeometryTorus::buildPrimitveTS(
    unsigned int aHorizontal,
    unsigned int aVertical,
    const MArrayVec3f& aPoints,
    const MArrayVec3f& aNormals,
    MRendererPrimitive& aOutPrimitive) const {
  aOutPrimitive.clearAll();
  aOutPrimitive.setMode(MRendererPrimitive::MODE_TRIANGLE_STRIP);
  aOutPrimitive.setDrawingMode(MRendererPrimitive::DRAWING_ARRAYS);

  for (unsigned int iy = 1; iy < aVertical; iy++) {
    for (unsigned int ix = 1; ix < aHorizontal; ix++) {
      unsigned int iPrev = (iy - 1) * aHorizontal + (ix - 1);
      unsigned int iCurr = iy * aHorizontal + ix;

      const MVector3f& vPrev = thePoints[iPrev];
      const MVector3f& vCurr = thePoints[iCurr];

      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vPrev);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vCurr);

      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iPrev]);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iCurr]);

      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vPrev[0] + 0.5f, vPrev[1] + 0.4f, vPrev[2] + 0.1f));
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vCurr[0] + 0.5f, vCurr[1] + 0.4f, vCurr[2] + 0.1f));
    }

    unsigned int iLast = (iy - 1) * aHorizontal + (aHorizontal - 1);
    unsigned int iFirstCurr = iy * aHorizontal + 0;
    unsigned int iFirstPrev = (iy - 1) * aHorizontal + 0;
    unsigned int iSecondCurr = iy * aHorizontal + 1;

    const MVector3f& vLast = thePoints[iLast];
    const MVector3f& vFirstCurr = thePoints[iFirstCurr];
    const MVector3f& vFirstPrev = thePoints[iFirstPrev];
    const MVector3f& vSecondCurr = thePoints[iSecondCurr];

    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vLast);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstPrev);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vSecondCurr);

    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iLast]);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iFirstCurr]);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iFirstPrev]);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iSecondCurr]);

    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vLast[0] + 0.5f, vLast[1] + 0.4f, vLast[2] + 0.1f));
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vFirstCurr[0] + 0.8f, vFirstCurr[1] + 0.4f, vFirstCurr[2] + 0.1f));
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vFirstPrev[0] + 0.8f, vFirstPrev[1] + 0.4f, vFirstPrev[2] + 0.1f));
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vSecondCurr[0] + 0.8f, vSecondCurr[1] + 0.4f, vSecondCurr[2] + 0.1f));

    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vSecondCurr);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iSecondCurr]);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(1.0f, 1.0f, 0.0f));
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iFirstCurr]);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(1.0f, 1.0f, 0.0f));
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iFirstCurr]);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(1.0f, 1.0f, 0.0f));
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iFirstCurr]);
    aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(1.0f, 1.0f, 0.0f));

    if (iy == (aVertical - 1)) {
      for (unsigned int ix = 1; ix < aHorizontal; ix++) {
        unsigned int iPrev = iy * aHorizontal + (ix - 1);
        unsigned int iCurr = 0 * aHorizontal + ix;

        const MVector3f& vPrev = thePoints[iPrev];
        const MVector3f& vCurr = thePoints[iCurr];

        aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vPrev);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vCurr);

        aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iPrev]);
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iCurr]);

        aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vPrev[0] + 0.5f, vPrev[1] + 0.4f, vPrev[2] + 0.1f));
        aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vCurr[0] + 0.5f, vCurr[1] + 0.4f, vCurr[2] + 0.1f));
      }

      unsigned int iLast = iy * aHorizontal + (aHorizontal - 1);
      unsigned int iFirstCurr = 0 * aHorizontal + 0;
      unsigned int iFirstPrev = iy * aHorizontal + 0;
      unsigned int iSecondCurr = 0 * aHorizontal + 1;

      const MVector3f& vLast = thePoints[iLast];
      const MVector3f& vFirstCurr = thePoints[iFirstCurr];
      const MVector3f& vFirstPrev = thePoints[iFirstPrev];
      const MVector3f& vSecondCurr = thePoints[iSecondCurr];

      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vLast);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstCurr);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vFirstPrev);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_VERTICES, vSecondCurr);

      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iLast]);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iFirstCurr]);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iFirstPrev]);
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_NORMALS, theNormals[iSecondCurr]);

      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vLast[0] + 0.5f, vLast[1] + 0.4f, vLast[2] + 0.1f));
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vFirstCurr[0] + 0.5f, vFirstCurr[1] + 0.4f, vFirstCurr[2] + 0.1f));
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vFirstPrev[0] + 0.5f, vFirstPrev[1] + 0.4f, vFirstPrev[2] + 0.1f));
      aOutPrimitive.append(MRendererPrimitive::ATTRIB_COLORS, MVector3f(vSecondCurr[0] + 0.5f, vSecondCurr[1] + 0.4f, vSecondCurr[2] + 0.1f));
    }
  }
}
