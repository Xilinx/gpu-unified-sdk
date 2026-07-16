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
 * @file MDRRenderer.h
 *
 * @brief This class represents a very simplified renderer object which holds several states such as:
 * frame number, current texture, viewport, projection matrix etc.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_RENDERER_HPP
#define M_SHADOWMAPDR_RENDERER_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"
#include "MMatrix.h"
#include "MDRRendererTexture.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * @brief Renderer class responsible for handling rendering operations and viewport management.
 * 
 * The `MDRRenderer` class manages the rendering process, including viewport dimensions,
 * projection matrices, and texture activation. It provides methods for initializing the
 * renderer, clearing the framebuffer, and updating frame counters.
 */
class MDRRenderer
{
public:

  // ----- Types -----

  /**
   * @brief Enumeration of texture units.
   * 
   * Defines the available texture units for texture activation.
   */
  enum MDRTexUnit
  {
    /** Texture unit 0 */
    TEXUNIT0,
    /** Texture unit 1 */
    TEXUNIT1,
    /** Texture unit 2 */
    TEXUNIT2,
    /** Texture unit 3 */
    TEXUNIT3
  };

  // ----- Constructors and destructors -----

  /**
   * @brief Constructs an MDRRenderer object.
   */
  MDRRenderer();

  /**
   * @brief Destructs the MDRRenderer object.
   */
  ~MDRRenderer();

  // ----- Accessors and mutators -----

  /**
   * @brief Returns the width of the current viewport.
   * 
   * @return The width of the viewport in pixels.
   */
  unsigned int getWidth() const
    { return theWidth; }

  /**
   * @brief Returns the height of the current viewport.
   * 
   * @return The height of the viewport in pixels.
   */
  unsigned int getHeight() const
    { return theHeight; }

  /**
   * @brief Returns the current projection matrix.
   * 
   * @return A constant reference to the current projection matrix.
   */
  const MMatrix4f& getMatrixP() const
    { return theMatrixP; }

  /**
   * @brief Returns the current frame counter value.
   * 
   * The frame counter is used for simple time measuring and tracking the number of frames rendered.
   * 
   * @return The current frame counter value.
   */
  unsigned int getFrameCounter() const
    { return theFrameCounter; }

  // ----- Miscellaneous -----

  /**
   * @brief Initializes the renderer with the specified viewport size.
   * 
   * Sets up the renderer with the given width and height, calculates the projection matrix, 
   * and performs any necessary initialization steps.
   * 
   * @param aWidth The width of the viewport in pixels.
   * @param aHeight The height of the viewport in pixels.
   * @return True if initialization was successful, false otherwise.
   */
  bool initialize(unsigned int aWidth, unsigned int aHeight);

  /**
   * @brief Prepares the renderer for a new frame.
   * 
   * This method resets all necessary states for the new frame and clears the framebuffer.
   */
  void preFrameRender();

  /**
   * @brief Finalizes rendering operations for the current frame.
   * 
   * This method is called after rendering each frame to update the frame counter.
   */
  void postFrameRender();

  /**
   * @brief Activates a texture on a specified texture unit.
   * 
   * This method binds the specified texture to the given texture unit for use in rendering.
   * 
   * @param aUnit The texture unit to activate (e.g., TEXUNIT0, TEXUNIT1).
   * @param aTexture The texture to activate.
   */
  void activateTexture(MDRTexUnit aUnit, const MDRRendererTexture& aTexture);

private:

  // ----- Fields -----

  /** Width of the viewport in pixels */
  unsigned int theWidth;

  /** Height of the viewport in pixels */
  unsigned int theHeight;

  /** Projection matrix used for rendering */
  MMatrix4f theMatrixP;

  /** Frame counter for time measurement */
  unsigned int theFrameCounter;

};

#endif /* M_SHADOWMAPDR_RENDERER_HPP */