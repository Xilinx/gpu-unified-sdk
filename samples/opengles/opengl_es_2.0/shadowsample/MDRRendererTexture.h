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
 * @file MDRRendererTexture.h
 *
 * @brief This class unlike the MDRFBO represents virtual render target based on 2D texture functionality. Instead of using FBO it simply copy framebuffer to texture.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_RENDERER_TEXTURE_HPP
#define M_SHADOWMAPDR_RENDERER_TEXTURE_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"
#include "MDRRenderable.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * @brief Represents a 2D texture for rendering, derived from `MDRRenderable`.
 * 
 * The `MDRRendererTexture` class handles the creation, binding, and management of 2D textures. 
 * It supports texture mipmapping and texture ring rotation for rendering operations.
 */
class MDRRendererTexture : public MDRRenderable
{
public:

  // ----- Types -----

  /**
   * @brief Base class type definition.
   * 
   * `MDRRendererTexture` inherits from `MDRRenderable`, and this typedef allows referencing 
   * the base class type for convenience.
   */
  typedef MDRRenderable BaseClass;

  // ----- Constructors and destructors -----

  /**
   * @brief Constructs an MDRRendererTexture object with optional mipmapping.
   * 
   * Initializes the texture object, optionally enabling mipmap generation.
   * 
   * @param aUseMipmap Whether to use mipmaps for the texture. Defaults to true.
   */
  MDRRendererTexture(bool aUseMipmap = true);

  /**
   * @brief Destructs the MDRRendererTexture object.
   * 
   * Cleans up resources associated with the texture.
   */
  virtual ~MDRRendererTexture();

  // ----- Accessors and mutators -----

  /**
   * @brief Returns the current native OpenGL name of the texture.
   * 
   * This name is used to identify the texture in OpenGL operations.
   * 
   * @return The current OpenGL texture name.
   */
  GLuint getName() const
    { return theName[theIndex]; }

  /**
   * @brief Checks if the texture has already been created.
   * 
   * Determines whether the texture has been successfully created and has a valid OpenGL name.
   * 
   * @return True if the texture is created, false otherwise.
   */
  bool isCreated() const
    { return (theName[0] != 0); }

  // ----- Miscellaneous -----

  /**
   * @brief Creates a 2D texture with specified size and data.
   * 
   * Allocates and initializes the texture with the provided width, height, and image data.
   * 
   * @param aWidth The width of the texture in pixels.
   * @param aHeight The height of the texture in pixels.
   * @param aData Pointer to the image data to be uploaded. If NULL, the texture will be created but not initialized.
   * @return True if the texture creation is successful, false otherwise.
   */
  bool create(unsigned int aWidth, unsigned int aHeight, const unsigned char* aData);

  /**
   * @brief Initializes the texture with default data.
   * 
   * Calls the `create` method with `aData` set to NULL, which initializes the texture without uploading any data.
   * 
   * @param aWidth The width of the texture in pixels.
   * @param aHeight The height of the texture in pixels.
   * @return True if the initialization is successful, false otherwise.
   */
  virtual bool initialize(unsigned int aWidth, unsigned int aHeight);

  /**
   * @brief Swaps the current texture index with the one to be updated.
   * 
   * Rotates the ring of textures used for rendering, effectively swapping the current texture with the next one.
   * 
   * @return True if the texture binding is successful, false otherwise.
   */
  virtual bool bindRT() const;

  /**
   * @brief Copies the framebuffer content into the texture.
   * 
   * Updates the texture with the current content of the framebuffer.
   */
  virtual void unbindRT() const;

  /**
   * @brief Binds the current texture for rendering.
   * 
   * Makes the current texture available for rendering on top of primitives.
   */
  virtual void bindTexture() const;

  /**
   * @brief Removes the texture.
   * 
   * Deletes the texture and releases associated resources.
   * 
   * @return True if the texture is successfully removed, false otherwise.
   */
  virtual bool destroy();

private:

  // ----- Fields -----

  /**
   * @brief Defines the number of textures in the ring.
   * 
   * The texture ring consists of a fixed number of textures, typically used for rotation.
   */
  enum
  {
    TEXTURES_COUNT = 2
  };

  /** The target texture type (e.g., GL_TEXTURE_2D) */
  const GLuint theTarget;

  /** The width of the texture in pixels */
  unsigned int theWidth;

  /** The height of the texture in pixels */
  unsigned int theHeight;

  /** Flag indicating whether mipmaps should be used */
  bool theUseMipmap;

  /** Current index of the texture in use */
  mutable unsigned int theIndex;

  /** Array of OpenGL texture names (handles) */
  GLuint theName[TEXTURES_COUNT];
};

#endif /* M_SHADOWMAPDR_RENDERER_TEXTURE_HPP */