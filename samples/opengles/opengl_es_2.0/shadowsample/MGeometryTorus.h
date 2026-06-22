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
 * @file MGeometryTorus.h
 *
 * @brief This class is inherited from a MGeometryBase class and represents a torus geometry.
 *
*******************************************************************************/

#ifndef M_SHADOWMAPDR_GEOMETRY_TORUS_HPP
#define M_SHADOWMAPDR_GEOMETRY_TORUS_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"
#include "MGeometryBase.h"
#include "MArray.h"
#include "MVector4.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * @brief Represents a torus geometric object.
 * 
 * The `MGeometryTorus` class extends the `MGeometryBase` class to define a torus geometry
 * with specified orbit and ring radii, and resolution. It allows for the creation of 
 * toroidal shapes with adjustable parameters.
 */
class MGeometryTorus : public MGeometryBase
{
public:

  // ----- Types -----

  /**
   * @brief Base class type alias.
   * 
   * Provides a type alias for the base class `MGeometryBase`.
   */
  typedef MGeometryBase BaseClass;

  /**
   * @brief Type alias for an array of 3D vectors.
   * 
   * Defines a type alias for an array containing `MVector3f` elements.
   */
  typedef MArray<MVector3f> MArrayVec3f;

  // ----- Constructors and destructors -----

  /**
   * @brief Default constructor for MGeometryTorus.
   * 
   * Initializes a new instance of the `MGeometryTorus` class with default values.
   */
  MGeometryTorus();

  /**
   * @brief Virtual destructor for MGeometryTorus.
   * 
   * Cleans up resources associated with the `MGeometryTorus` instance.
   */
  virtual ~MGeometryTorus();

  // ----- Operators -----

  // (No operator overloads defined in this class.)

  // ----- Accessors and mutators -----

  // (No additional accessors or mutators defined in this class.)

  // ----- Miscellaneous -----

  /**
   * @brief Builds a torus geometry with specified orbit and ring radii.
   * 
   * Configures the torus with given orbit and ring radii, and defines the resolution
   * by specifying the number of horizontal and vertical segments. This method sets up
   * the points and normals for the torus geometry.
   * 
   * @param aRadiusOrbit The radius of the orbit of the torus.
   * @param aRadiusRing The radius of the ring of the torus.
   * @param aHorizontal The number of horizontal segments for the torus geometry.
   * @param aVertical The number of vertical segments for the torus geometry.
   */
  void set(float aRadiusOrbit,
           float aRadiusRing,
           unsigned int aHorizontal,
           unsigned int aVertical);

private:

  // ----- Fields -----

  /**
   * @brief Array of 3D points defining the torus geometry.
   * 
   * Stores the vertices of the torus.
   */
  MArrayVec3f thePoints;

  /**
   * @brief Array of 3D normals for the torus geometry.
   * 
   * Stores the normals corresponding to the vertices of the torus.
   */
  MArrayVec3f theNormals;

  // ----- Miscellaneous -----

  /**
   * @brief Builds a torus primitive using a triangle strip based on indices.
   * 
   * Constructs the torus geometry using a triangle strip with indexed vertices and normals.
   * 
   * @param aHorizontal The number of horizontal segments for the torus geometry.
   * @param aVertical The number of vertical segments for the torus geometry.
   * @param aPoints The array of 3D points defining the torus.
   * @param aNormals The array of 3D normals for the torus.
   * @param aOutPrimitive The output primitive to be populated with the torus data.
   */
  void buildPrimitveTSI(unsigned int aHorizontal,
                        unsigned int aVertical,
                        const MArrayVec3f& aPoints,
                        const MArrayVec3f& aNormals,
                        MRendererPrimitive& aOutPrimitive) const;

  /**
   * @brief Builds a torus primitive using a triangle strip.
   * 
   * Constructs the torus geometry using a triangle strip without indexed vertices.
   * 
   * @param aHorizontal The number of horizontal segments for the torus geometry.
   * @param aVertical The number of vertical segments for the torus geometry.
   * @param aPoints The array of 3D points defining the torus.
   * @param aNormals The array of 3D normals for the torus.
   * @param aOutPrimitive The output primitive to be populated with the torus data.
   */
  void buildPrimitveTS(unsigned int aHorizontal,
                       unsigned int aVertical,
                       const MArrayVec3f& aPoints,
                       const MArrayVec3f& aNormals,
                       MRendererPrimitive& aOutPrimitive) const;
};

#endif /* M_SHADOWMAPDR_GEOMETRY_TORUS_HPP */