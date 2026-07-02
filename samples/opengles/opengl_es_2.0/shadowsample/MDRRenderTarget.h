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
 * @file MDRRendererTarget.h
 *
 * @brief This class represents two types of render targets such as: Frame Buffer Object and TextureEngine. When the FBO mode is used all graphics go to the Frame Buffer Object,
 * whereas the Copy TextureEngine mode is used all graphics is first rendered to the framebuffer and then copied to the texture.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_RENDER_TARGET_HPP
#define M_SHADOWMAPDR_RENDER_TARGET_HPP

#include "ShadowMap.h"
#include "MDRRenderable.h"

/**
 * @brief Represents a render target, either an FBO (Framebuffer Object) or a texture.
 * 
 * The `MDRRenderTarget` class provides functionalities for creating, managing, and using render targets
 * in the rendering pipeline. It supports both framebuffer objects (FBO) and texture-based render targets.
 */
class MDRRenderTarget : public MDRRenderable
{
public:

  // ----- Types -----

  /**
   * @brief Defines the mode of the render target.
   * 
   * This enum specifies the type of render target to use.
   */
  enum MDRMode
  {
    /** Use a Framebuffer Object (FBO) as the render target. */
    MODE_FBO,
    /** Use a texture as the render target. */
    MODE_COPY_TEXTURE
  };

  /**
   * @brief Base class type definition.
   * 
   * `MDRRenderTarget` inherits from `MDRRenderable`, and this typedef allows referencing 
   * the base class type for convenience.
   */
  typedef MDRRenderable BaseClass;

  // ----- Constructors and destructors -----

  /**
   * @brief Constructs an MDRRenderTarget object with a specified mode.
   * 
   * Initializes the render target with the given mode, determining whether it will use an FBO or a texture.
   * 
   * @param aMode The mode of the render target (FBO or texture).
   */
  MDRRenderTarget(MDRMode aMode);

  /**
   * @brief Destructs the MDRRenderTarget object.
   * 
   * Cleans up resources associated with the render target.
   */
  virtual ~MDRRenderTarget();

  // ----- Accessors and mutators -----

  // ----- Miscellaneous -----

  /**
   * @brief Creates the render target with the specified size.
   * 
   * Allocates and initializes the render target based on the mode specified during construction.
   * 
   * @param aWidth The width of the render target in pixels.
   * @param aHeight The height of the render target in pixels.
   * @return True if the initialization is successful, false otherwise.
   */
  virtual bool initialize(unsigned int aWidth, unsigned int aHeight);

  /**
   * @brief Activates the render target if using FBO mode.
   * 
   * Binds the FBO as the current render target if FBO mode is active. Otherwise, this method does nothing.
   * 
   * @return True if the bind operation is successful in FBO mode, false otherwise.
   */
  virtual bool bindRT() const;

  /**
   * @brief Deactivates the render target and binds the default framebuffer.
   * 
   * Unbinds the current render target and sets the default framebuffer (0) as the current render target.
   */
  virtual void unbindRT() const;

  /**
   * @brief Binds the render target as a texture.
   * 
   * Makes the render target available as a texture for use in rendering operations.
   */
  virtual void bindTexture() const;

  /**
   * @brief Releases GPU resources associated with the render target.
   * 
   * Cleans up and deletes the resources associated with either the FBO or texture, depending on the mode.
   * 
   * @return True if the resource release is successful, false otherwise.
   */
  virtual bool destroy();

private:

  // ----- Fields -----

  /** The width of the render target in pixels */
  unsigned int theWidth;

  /** The height of the render target in pixels */
  unsigned int theHeight;

  /** Pointer to the renderable object used for the render target */
  MDRRenderable* theRenderable;
};

#endif /* M_SHADOWMAPDR_RENDER_TARGET_HPP */