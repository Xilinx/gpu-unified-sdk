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
 * @file MRendererPrimitive.cpp
 *
 * @brief  This class represents data of vertices and theirs attributes.
 *
*******************************************************************************/

#include "MRendererPrimitive.h"

using namespace SDKXilinx;

MRendererPrimitive::MRendererPrimitive()
    :
    theMode(MODE_TRIANGLES),
    theDrawingMode(DRAWING_ARRAYS),
    theBufferHandle(0),
    //
    theData()
  {
  memset(theAttribs, -1, sizeof(theAttribs));
  }

MRendererPrimitive::~MRendererPrimitive()
  {
  }

void MRendererPrimitive::append(
    MDRAttrib aAttrib,
    const MVector3f& aValue)
  {
  theData[aAttrib].append(aValue);
  }

bool MRendererPrimitive::append(
    const MRendererPrimitive& aOther)
  {
  // To merge both primitives they have to be in the same mode
  if (theMode != aOther.theMode ||
      theDrawingMode != aOther.theDrawingMode)
    {
    LOGI("Warning: Cannot merge primitive with different structure (modes)!");
    return false;
    }

  unsigned int thisCountOfVertices = theData[ATTRIB_VERTICES].getCount();
  unsigned int otherCount = aOther.theData[ATTRIB_VERTICES].getCount();
  // Merge Vertices, Normals, Colors etc.
  for (unsigned int attrib = 0; attrib < ATTRIBS_COUNT; attrib++)
    theData[attrib].append(aOther.theData[attrib]);

  // Merge Indices
  if (theIndices.getCount() > 0)
    {
    // Add two dummy last indices to not drawing primitive
    theIndices.append(theIndices[theIndices.getCount() - 1]);
    theIndices.append(theIndices[theIndices.getCount() - 1]);
    }
  if (aOther.theIndices.getCount() > 0)
    {
    // Add two dummy last indices to not drawing primitive
    theIndices.append(aOther.theIndices[0] + thisCountOfVertices);
    theIndices.append(aOther.theIndices[0] + thisCountOfVertices);
    }
  unsigned int countOfIndices = aOther.theIndices.getCount();
  for (unsigned int index = 0; index < countOfIndices; index++)
    theIndices.append(aOther.theIndices[index] + thisCountOfVertices);

  return true;
  }

void MRendererPrimitive::clearAll()
  {
  for (unsigned int attrib = 0; attrib < ATTRIBS_COUNT; attrib++)
    theData[attrib].clear();
  theIndices.clear();
  }

bool MRendererPrimitive::setAttribIndex(
    MDRAttrib aAttrib,
    int aIndex)
  {
  if (aAttrib >= ATTRIBS_COUNT)
    return false;
  theAttribs[aAttrib] = aIndex;
  return true;
  }

void MRendererPrimitive::render()
  {
  for (unsigned int attrib = 0; attrib < ATTRIBS_COUNT; attrib++)
    {
    if (theAttribs[attrib] >= 0)
      {
      GL_CHECK(glEnableVertexAttribArray(theAttribs[attrib]));
      GL_CHECK(glVertexAttribPointer(theAttribs[attrib], 3, GL_FLOAT, GL_FALSE, 0, theData[attrib].getData()));
      }
    }
  //
  if (theDrawingMode == DRAWING_ELEMENTS &&
      theIndices.getCount())
    {
    GL_CHECK(glDrawElements(theMode,
                            theIndices.getCount(),
                            GL_UNSIGNED_SHORT,
                            theIndices.getData()));
    }
  else
    {
    GL_CHECK(glDrawArrays(theMode, 0, theData[ATTRIB_VERTICES].getCount()));
    }
  //
  for (unsigned int attrib = 0; attrib < ATTRIBS_COUNT; attrib++)
    {
    if (theAttribs[attrib] >= 0)
      {
      GL_CHECK(glDisableVertexAttribArray(theAttribs[attrib]));
      }
    }
  }

void MRendererPrimitive::transform(
    const MVector3f& aPosition)
  {
  // Transform vertices
  MArrayf::MDRIterator it = theData[ATTRIB_VERTICES].getBegin();
  MArrayf::MDRIterator end = theData[ATTRIB_VERTICES].getEnd();
  while (it != end)
    {
    MVector3f& vertex = *it;
    vertex += aPosition;
    it++;
    }
  }
