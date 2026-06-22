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
 * @file MGeometryBase.cpp
 *
 * @brief This class is a base for other geometries such as: sphere, cone, cube and so on.
 * The RAW geometry is held by thePrimitive member type of MRendererPrimitive.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_GEOMETRY_BASE_HPP
#define M_SHADOWMAPDR_GEOMETRY_BASE_HPP

#include "ShadowMap.h"
#include "MArray.h"
#include "MRendererPrimitive.h"
#include "MVector4.h"

class MRendererProgram;

/**
 * @brief Represents the base class for geometric objects in the rendering system.
 * 
 * The `MGeometryBase` class provides a base implementation for geometric objects, 
 * encapsulating a primitive shape and offering methods to access and render it.
 */
class MGeometryBase
{
public:

  // ----- Types -----

  // (No specific types defined in this class.)

  // ----- Constructors and destructors -----

  /**
   * @brief Default constructor for MGeometryBase.
   * 
   * Initializes a new instance of the `MGeometryBase` class.
   */
  MGeometryBase();

  /**
   * @brief Virtual destructor for MGeometryBase.
   * 
   * Cleans up resources associated with the `MGeometryBase` instance.
   */
  virtual ~MGeometryBase();

  // ----- Operators -----

  // (No operator overloads defined in this class.)

  // ----- Accessors and mutators -----

  /**
   * @brief Returns a constant reference to the primitive.
   * 
   * Provides read-only access to the underlying `MRendererPrimitive` object.
   * 
   * @return A constant reference to the primitive.
   */
  const MRendererPrimitive& getPrimitive() const
    { return thePrimitive; }

  /**
   * @brief Returns a reference to the primitive for modification.
   * 
   * Provides read-write access to the underlying `MRendererPrimitive` object,
   * allowing modifications to the shape of the primitive.
   * 
   * @return A reference to the primitive.
   */
  MRendererPrimitive& getPrimitive()
    { return thePrimitive; }

  // ----- Miscellaneous -----

  /**
   * @brief Renders the primitive using the specified rendering program.
   * 
   * Executes the rendering of the primitive with the provided `MRendererProgram`,
   * allowing for customized rendering behavior.
   * 
   * @param aProgram The rendering program used for rendering the primitive.
   */
  virtual void render(const MRendererProgram& aProgram);

private:

  // ----- Fields -----

  /** The primitive shape associated with the geometry. */
  MRendererPrimitive thePrimitive;

};

#endif /* M_SHADOWMAPDR_GEOMETRY_BASE_HPP */