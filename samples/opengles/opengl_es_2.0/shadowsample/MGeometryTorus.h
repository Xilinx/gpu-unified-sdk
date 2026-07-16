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