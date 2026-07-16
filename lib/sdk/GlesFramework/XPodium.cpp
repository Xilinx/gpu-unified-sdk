/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/**
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 *
*******************************************************************************/
/**
 *
 * @file XPodium.cpp
 *
 * @brief This file implements all the functions related to podium baseclass.
 *
*******************************************************************************/


#include "XPodium.h"

#include <cstdio>
#include <cstdarg>

XPodium* XPodium::getHandler()
{
	#ifdef ENABLE_FBDEV
	return XLinuxMali::getHandler();
    #endif
	
	#ifdef ENABLE_X11
	return XLinuxPodium::getHandler();
    #endif

	#ifdef ENABLE_WAYLAND
    return XWaylandPodium::getHandler();
    #endif
}

void XPodium::log(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

