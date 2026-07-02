/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/******************************************************************************/
/**
 * @cond CC
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        -----------------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 *
*******************************************************************************/
/******************************************************************************/
/**
 *
 * @file Xfbdev_window.h
 *
 * This file implements all the functions related to podium baseclass.
 *
********************************************************************************/
#ifndef _FBDEV_WINDOW_H_
#define _FBDEV_WINDOW_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct fbdev_window
 * @brief Represents a window for framebuffer device (fbdev) usage.
 * 
 * This structure is used to define the dimensions of a window when working with
 * framebuffer devices. It includes the width and height of the window in pixels.
 */
typedef struct fbdev_window
{
    unsigned short width;  ///< The width of the window in pixels.
    unsigned short height; ///< The height of the window in pixels.
} fbdev_window;

#ifdef __cplusplus
}
#endif

#endif /* _FBDEV_WINDOW_H_ */