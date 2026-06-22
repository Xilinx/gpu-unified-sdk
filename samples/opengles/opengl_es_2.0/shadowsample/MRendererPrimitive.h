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
 * @file MRendererPrimitive.h
 *
 * @brief  This class represents data of vertices and theirs attributes.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_RENDERER_PRIMITIVE_HPP
#define M_SHADOWMAPDR_RENDERER_PRIMITIVE_HPP


#include "ShadowMap.h"
#include "MArray.h"
#include "MVector4.h"

class MRendererPrimitive
{
public:

  /**
   * @brief Enumeration of primitive drawing modes.
   * 
   * Defines different modes for drawing primitives in OpenGL.
   */
  enum MMode
  {
    /// Draws primitives as triangles.
    MODE_TRIANGLES = GL_TRIANGLES,

    /// Draws primitives as a triangle strip.
    MODE_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,

    /// Draws primitives as points.
    MODE_POINTS = GL_POINTS,

    /// Draws primitives as a line strip.
    MODE_LINE_STRIP = GL_LINE_STRIP
  };

  /**
   * @brief Enumeration of drawing modes.
   * 
   * Specifies how the primitives are drawn: either using arrays or elements.
   */
  enum MDRDrawingMode
  {
    /// Draws primitives using vertex arrays.
    DRAWING_ARRAYS,

    /// Draws primitives using element arrays.
    DRAWING_ELEMENTS
  };

  /**
   * @brief Enumeration of attribute types.
   * 
   * Defines different types of vertex attributes that can be used.
   */
  enum MDRAttrib
  {
    /// Vertex positions.
    ATTRIB_VERTICES,

    /// Vertex colors.
    ATTRIB_COLORS,

    /// Vertex normals.
    ATTRIB_NORMALS,

    /// First set of texture coordinates.
    ATTRIB_TEXCOORDS_0,

    /// Second set of texture coordinates.
    ATTRIB_TEXCOORDS_1,

    /// Third set of texture coordinates.
    ATTRIB_TEXCOORDS_2,

    /// Fourth set of texture coordinates.
    ATTRIB_TEXCOORDS_3,

    /// Fifth set of texture coordinates.
    ATTRIB_TEXCOORDS_4,

    /// Sixth set of texture coordinates.
    ATTRIB_TEXCOORDS_5,

    /// Seventh set of texture coordinates.
    ATTRIB_TEXCOORDS_6,

    /// Eighth set of texture coordinates.
    ATTRIB_TEXCOORDS_7,

    /// Total number of attributes.
    ATTRIBS_COUNT
  };

  /**
   * @brief Default constructor for MRendererPrimitive.
   * 
   * Initializes an empty MRendererPrimitive instance with default values.
   */
  MRendererPrimitive();

  /**
   * @brief Destructor for MRendererPrimitive.
   * 
   * Cleans up resources used by the MRendererPrimitive instance.
   */
  ~MRendererPrimitive();

  /**
   * @brief Sets the primitive drawing mode.
   * 
   * Configures the mode used for rendering primitives (e.g., triangles, points).
   * 
   * @param aMode The mode to set for drawing primitives.
   */
  void setMode(MMode aMode)
    { theMode = aMode; }

  /**
   * @brief Sets the drawing mode for primitives.
   * 
   * Specifies whether primitives are drawn using arrays or elements.
   * 
   * @param aDrawingMode The drawing mode to use.
   */
  void setDrawingMode(MDRDrawingMode aDrawingMode)
    { theDrawingMode = aDrawingMode; }

  // ----- Miscellaneous -----

  /**
   * @brief Adds an attribute value to the list of attributes.
   * 
   * Appends the specified attribute value (e.g., vertex position, color) to the corresponding
   * list of attributes.
   * 
   * @param aAttrib The type of attribute to add.
   * @param aBox The attribute value to add.
   */
  void append(MDRAttrib aAttrib,
              const MVector3f& aBox);

  /**
   * @brief Appends a single index to the list of indices.
   * 
   * Adds an index to the list used for indexed drawing of primitives.
   * 
   * @param aIndex The index to add.
   */
  void appendIndex(unsigned short aIndex)
    { theIndices.append(aIndex); }

  /**
   * @brief Appends another primitive to this one.
   * 
   * Merges the attributes and indices of another primitive into this one. The other primitive
   * must have the same mode as this one.
   * 
   * @param aOther The primitive to append.
   * 
   * @return True if the append operation is successful, false otherwise.
   */
  bool append(const MRendererPrimitive& aOther);

  /**
   * @brief Clears all attributes and indices.
   * 
   * Removes all vertex attributes and indices from this primitive.
   */
  void clearAll();

  /**
   * @brief Sets attribute indices reported by the shader program.
   * 
   * Configures the attribute indices to match those reported by the shader program.
   * 
   * @param aAttrib The attribute to set the index for.
   * @param aIndex The index to set.
   * 
   * @return True if the operation is successful, false otherwise.
   */
  bool setAttribIndex(MDRAttrib aAttrib,
                      int aIndex);

  /**
   * @brief Renders the primitive using OpenGL.
   * 
   * Configures vertex pointers and issues a draw call to render the primitive.
   */
  void render();

  /**
   * @brief Transforms all vertices by a specified position vector.
   * 
   * Applies a translation transformation to all vertices in the primitive.
   * The transformation is performed on the CPU.
   * 
   * @param aPosition The position vector to translate vertices by.
   */
  void transform(const MVector3f& aPosition);

private:

  // ----- Types -----

  /// Type definition for an array of 3D vectors.
  typedef MArray<MVector3f> MArrayf;

  /// Type definition for an array of unsigned short indices.
  typedef MArray<unsigned short> MArrayus;

  // ----- Fields -----

  /// The mode used for rendering primitives.
  MMode theMode;

  /// The drawing mode used (arrays or elements).
  MDRDrawingMode theDrawingMode;

  /// Handle to the buffer used for storing vertex data.
  GLuint theBufferHandle;

  /// Array of attributes, each represented by a list of 3D vectors.
  MArrayf theData[ATTRIBS_COUNT];

  /// List of indices used for indexed drawing.
  MArrayus theIndices;

  /// Array of attribute indices as reported by the shader program.
  int theAttribs[ATTRIBS_COUNT];
};

#endif
