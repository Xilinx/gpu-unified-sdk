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
 * @file MDRRenderable.h
 *
 * @brief This class represents an abstract object, which could be used as a render target (e.g. FBO).
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_RENDERABLE_HPP
#define M_SHADOWMAPDR_RENDERABLE_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"
#include "MDRRenderable.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * @brief Abstract base class representing a renderable object.
 * 
 * This class defines an interface for objects that can be used as render targets,
 * such as Framebuffer Objects (FBOs). It provides pure virtual methods that must
 * be implemented by derived classes to handle rendering operations.
 */
class MDRRenderable
{
public:

  // ----- Constructors and destructors -----

  /**
   * @brief Constructs an MDRRenderable object.
   */
  MDRRenderable();

  /**
   * @brief Destructs the MDRRenderable object.
   */
  virtual ~MDRRenderable();

  // ----- Accessors and mutators -----

  // No accessors and mutators are defined in this base class.

  // ----- Miscellaneous -----

  /**
   * @brief Initializes the renderable object with specified dimensions.
   * 
   * This method must be implemented by derived classes to set up the rendering target
   * with the given width and height.
   * 
   * @param aWidth The width of the renderable object.
   * @param aHeight The height of the renderable object.
   * @return True if initialization was successful, false otherwise.
   */
  virtual bool initialize(unsigned int aWidth, unsigned int aHeight) = 0;

  /**
   * @brief Binds the render target for rendering operations.
   * 
   * This method must be implemented by derived classes to bind the render target
   * so that rendering operations are directed to it.
   * 
   * @return True if the binding was successful, false otherwise.
   */
  virtual bool bindRT() const = 0;

  /**
   * @brief Unbinds the render target.
   * 
   * This method must be implemented by derived classes to unbind the render target
   * after rendering operations are completed.
   */
  virtual void unbindRT() const = 0;

  /**
   * @brief Binds the texture associated with the renderable object.
   * 
   * This method must be implemented by derived classes to bind the texture for use
   * in rendering or shader operations.
   */
  virtual void bindTexture() const = 0;

  /**
   * @brief Destroys the renderable object and cleans up resources.
   * 
   * This method must be implemented by derived classes to properly release any resources
   * and perform necessary clean-up operations.
   * 
   * @return True if destruction was successful, false otherwise.
   */
  virtual bool destroy() = 0;

private:

  // ----- Fields -----

  // No private fields are defined in this base class.

};

#endif /* M_SHADOWMAPDR_RENDERABLE_HPP */