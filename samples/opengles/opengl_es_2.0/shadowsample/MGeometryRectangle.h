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
 * @file MGeometryRectangle.cpp
 *
 * @brief This class is inherited from a MGeometryBase class and represents a rectangle geometry.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_GEOMETRY_RECTANGLE_HPP
#define M_SHADOWMAPDR_GEOMETRY_RECTANGLE_HPP

#include "ShadowMap.h"
#include "MGeometryBase.h"

/**
 * @brief Represents a rectangular geometric object.
 * 
 * The `MGeometryRectangle` class extends the `MGeometryBase` class to define a rectangle
 * geometry with specified width, height, and resolution. This class allows the creation 
 * and manipulation of a rectangle's geometry by specifying its dimensions and resolution.
 */
class MGeometryRectangle : public MGeometryBase
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
   * @brief Default constructor for MGeometryRectangle.
   * 
   * Initializes a new instance of the `MGeometryRectangle` class with default values.
   */
  MGeometryRectangle();

  /**
   * @brief Virtual destructor for MGeometryRectangle.
   * 
   * Cleans up resources associated with the `MGeometryRectangle` instance.
   */
  virtual ~MGeometryRectangle();

  // ----- Operators -----

  // (No operator overloads defined in this class.)

  // ----- Accessors and mutators -----

  // (No additional accessors or mutators defined in this class.)

  // ----- Miscellaneous -----

  /**
   * @brief Builds a rectangle geometry with specified dimensions and resolution.
   * 
   * Configures the rectangle's width and height, and specifies the resolution
   * by defining the number of horizontal and vertical segments.
   * 
   * @param aWidth The width of the rectangle.
   * @param aHeight The height of the rectangle.
   * @param aHorizontal The number of horizontal segments for the rectangle's geometry.
   * @param aVertical The number of vertical segments for the rectangle's geometry.
   */
  void set(float aWidth,
           float aHeight,
           unsigned int aHorizontal,
           unsigned int aVertical);

private:

  // ----- Fields -----

  // (No additional fields defined in this class.)

};

#endif /* M_SHADOWMAPDR_GEOMETRY_RECTANGLE_HPP */