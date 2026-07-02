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