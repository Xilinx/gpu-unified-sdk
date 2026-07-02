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
 * @file MGeometryComplex.h
 *
 * @brief This class represents a complex geometry. A user can merge various primitives into
 * one object, which is represented by this class. In other words a user can combine multiple draw calls into one by using this class.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_GEOMETRY_COMPLEX_HPP
#define M_SHADOWMAPDR_GEOMETRY_COMPLEX_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"
#include "MGeometryBase.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * @brief Represents a complex geometric object composed of multiple primitives.
 * 
 * The `MGeometryComplex` class extends the `MGeometryBase` class to support more complex
 * geometric configurations by allowing the addition and modification of multiple
 * `MRendererPrimitive` objects.
 */
class MGeometryComplex : public MGeometryBase
{
public:

  // ----- Types -----

  /**
   * @brief Base class type alias.
   * 
   * Provides a type alias for the base class `MGeometryBase`.
   */
  typedef MGeometryBase BaseClass;

  // ----- Constructors and destructors -----

  /**
   * @brief Default constructor for MGeometryComplex.
   * 
   * Initializes a new instance of the `MGeometryComplex` class.
   */
  MGeometryComplex();

  /**
   * @brief Virtual destructor for MGeometryComplex.
   * 
   * Cleans up resources associated with the `MGeometryComplex` instance.
   */
  virtual ~MGeometryComplex();

  // ----- Operators -----

  // (No operator overloads defined in this class.)

  // ----- Accessors and mutators -----

  // (No additional accessors or mutators defined in this class.)

  // ----- Miscellaneous -----

  /**
   * @brief Replaces the current primitive with a new one.
   * 
   * Sets the primitive shape of the geometry to the provided `MRendererPrimitive`,
   * including its associated modes.
   * 
   * @param aPrimitive The new `MRendererPrimitive` to replace the current one.
   */
  void set(const MRendererPrimitive& aPrimitive)
    { getPrimitive() = aPrimitive; }

  /**
   * @brief Appends a new primitive to the existing geometry.
   * 
   * Merges the provided `MRendererPrimitive` with the existing mesh represented
   * by this `MGeometryComplex` object.
   * 
   * @param aPrimitive The `MRendererPrimitive` to be appended to the mesh.
   */
  void append(const MRendererPrimitive& aPrimitive)
    { getPrimitive().append(aPrimitive); }

private:

  // ----- Fields -----

  // (No additional fields defined in this class.)

};

#endif /* M_SHADOWMAPDR_GEOMETRY_COMPLEX_HPP */