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
 * @file MDRRendererProgram.cpp
 *
 * @brief This class is a specialized wrapper over vertex and fragment programs and parameters associated to them just for this example purpose.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_RENDERER_PROGRAM_HPP
#define M_SHADOWMAPDR_RENDERER_PROGRAM_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"
#include "MArray.h"
#include "MMatrix.h"
#include "MString.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * @brief Represents a shader program in a rendering pipeline.
 * 
 * The `MDRRendererProgram` class manages OpenGL shader programs, including vertex and fragment shaders.
 * It handles attribute and uniform locations, allowing for setting values and using the shader program
 * in rendering operations.
 */
class MDRRendererProgram
{
public:

  // ----- Types -----

  /**
   * @brief Enumeration of attribute locations in the shader program.
   * 
   * Defines the locations for various vertex attributes like position, color, and texture coordinates.
   */
  enum MDRAttribLocation
  {
    /** Location of the vertex attribute */
    ATTRIB_LOC_VERTEX = 0,
    /** Location of the color attribute */
    ATTRIB_LOC_COLOR = 1,
    /** Location of the texture coordinate attribute 0 */
    ATTRIB_LOC_TEXCOORD0 = 2,
    /** Total number of attribute locations */
    ATTRIBS_COUNT = 3
  };

  /**
   * @brief Enumeration of uniform locations in the shader program.
   * 
   * Defines the locations for various uniform variables such as matrices and texture samplers.
   */
  enum MDRUniformLocation
  {
    /** Location of the Model-View-Projection matrix */
    UNIFORM_LOC_MATRIX_MVP = 0,
    /** Location of texture matrix 0 */
    UNIFORM_LOC_MATRIX_T0 = 1,
    /** Location of texture sampler 0 */
    UNIFORM_LOC_SAMPLER0 = 2,
    /** Total number of uniform locations */
    UNIFORM_LOC_COUNT = 3
  };

  /**
   * @brief Type definition for shader location values.
   * 
   * This type represents the integer values used for attribute and uniform locations.
   */
  typedef GLint MDRLocValueType;

  // ----- Constructors and destructors -----

  /**
   * @brief Constructs an MDRRendererProgram object.
   * 
   * Initializes internal state but does not load or compile shaders.
   */
  MDRRendererProgram();

  /**
   * @brief Destructs the MDRRendererProgram object.
   * 
   * Cleans up resources associated with the shader program.
   */
  ~MDRRendererProgram();

  // ----- Accessors and mutators -----

  /**
   * @brief Returns the native handle for a given attribute location.
   * 
   * This handle is used for binding vertex attributes such as positions, colors, and texture coordinates.
   * 
   * @param aAttribLocationType The attribute location type to retrieve.
   * @return The native handle for the specified attribute location.
   */
  MDRLocValueType getLocAttrib(MDRAttribLocation aAttribLocationType) const
    { return theLocAttrib[aAttribLocationType]; }

  /**
   * @brief Returns the native handle for a given uniform location.
   * 
   * This handle is used for setting uniform variables such as matrices and texture samplers.
   * 
   * @param aUniformLocationType The uniform location type to retrieve.
   * @return The native handle for the specified uniform location.
   */
  MDRLocValueType getLocUniform(MDRUniformLocation aUniformLocationType) const
    { return theLocUniform[aUniformLocationType]; }

  // ----- Miscellaneous -----

  /**
   * @brief Loads vertex and fragment shaders from specified file paths and sets up attribute and uniform locations.
   * 
   * Compiles shaders from the given file paths and links them into a shader program. Also retrieves and stores
   * the locations of attributes and uniforms.
   * 
   * @param aPathVP The file path to the vertex shader.
   * @param aPathFP The file path to the fragment shader.
   * @return True if initialization was successful, false otherwise.
   */
  bool initialize(const MPath& aPathVP, const MPath& aPathFP);

  /**
   * @brief Binds the shader program for use in rendering.
   * 
   * Activates the shader program, making it the current program for rendering operations.
   */
  void use() const;

  /**
   * @brief Sets a matrix value for a specific uniform parameter in the shader program.
   * 
   * Updates the value of a uniform matrix variable in the shader program.
   * 
   * @param aUniformLocationType The location of the uniform to update.
   * @param aMatrix The matrix value to set.
   */
  void setUniform(MDRUniformLocation aUniformLocationType, const MMatrix4f& aMatrix) const;

  /**
   * @brief Sets an integer value for a specific uniform parameter in the shader program.
   * 
   * Updates the value of a uniform integer variable in the shader program.
   * 
   * @param aUniformLocationType The location of the uniform to update.
   * @param aValue The integer value to set.
   */
  void setUniform(MDRUniformLocation aUniformLocationType, int aValue) const;

private:

  // ----- Types -----

  /**
   * @brief Type definition for an array of shader location values.
   * 
   * An array to store location values for attributes and uniforms.
   */
  typedef MArray<MDRLocValueType> MDRLocArray;

  // ----- Fields -----

  /** The OpenGL handle for the shader program */
  GLuint theProgram;

  /** The OpenGL handle for the vertex shader */
  GLuint theProgramVertex;

  /** The OpenGL handle for the fragment shader */
  GLuint theProgramFragment;

  /** Array of attribute location values */
  MDRLocArray theLocAttrib;

  /** Array of uniform location values */
  MDRLocArray theLocUniform;
};

#endif /* M_SHADOWMAPDR_RENDERER_PROGRAM_HPP */