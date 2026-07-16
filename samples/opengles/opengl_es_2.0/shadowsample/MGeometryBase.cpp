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
 * @file MGeometryBase.cpp
 *
 * @brief This class is a base for other geometries such as: sphere, cone, cube and so on.
 * The RAW geometry is held by thePrimitive member type of MRendererPrimitive.
 *
*******************************************************************************/

#include "MGeometryBase.h"

#include "MRendererProgram.h"

MGeometryBase::MGeometryBase()
    :
    thePrimitive()
  {
  }

MGeometryBase::~MGeometryBase()
  {
  }

void MGeometryBase::render(
    const MRendererProgram& aProgram)
  {
  thePrimitive.setAttribIndex(MRendererPrimitive::ATTRIB_VERTICES,   aProgram.getLocAttrib(MRendererProgram::A_LOC_VERTEX));
  thePrimitive.setAttribIndex(MRendererPrimitive::ATTRIB_COLORS,     aProgram.getLocAttrib(MRendererProgram::A_LOC_COLOR));
  thePrimitive.setAttribIndex(MRendererPrimitive::ATTRIB_NORMALS,    aProgram.getLocAttrib(MRendererProgram::A_LOC_NORMAL));
  thePrimitive.setAttribIndex(MRendererPrimitive::ATTRIB_TEXCOORDS_0,aProgram.getLocAttrib(MRendererProgram::A_LOC_TEXCOORD_0));
  //
  thePrimitive.render();
  }
