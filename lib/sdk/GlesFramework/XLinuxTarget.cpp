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

 * @file XLinuxTarget.cpp
 *
 * This file implements all the functions related to Linux XPodium class for application.
 *
*******************************************************************************/



#include <cstdlib>
#include <sys/fcntl.h>
#include <sys/unistd.h>

#include "XLinuxTarget.h"

    XPodium* XLinuxMali::instance = NULL;

    XLinuxMali::XLinuxMali(void)
    {

    }

    XPodium* XLinuxMali::getHandler(void)
    {
        if (instance == NULL)
        {
            instance = new XLinuxMali();
        }
        return instance;
    }

    void XLinuxMali::prepareWindow(int width, int height)
    {
        Fwindow = (fbdev_window *)calloc(1, sizeof(fbdev_window));
        if(Fwindow == NULL)
        {
            printf("Out of memory at %s:%i\n", __FILE__, __LINE__);
            exit(1);
        }
        Fwindow->width = width;
        Fwindow->height = height;
    }

    void XLinuxMali::destroyWindow(void)
    {
        free(Fwindow);
    }

    XPodium::WindowStatus XLinuxMali::checkWindow(void)
    {
        return XPodium::WINDOW_IDLE;
    }
