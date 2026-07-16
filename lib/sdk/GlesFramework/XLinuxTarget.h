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

 * @file XLinuxTarget.h
 *
 * This file implements all the functions related to Linux XPodium class for application.
 *
*******************************************************************************/


#ifndef XLINUXTARGET_H
#define XLINUXTARGET_H

#include <cstdlib>
#include "XPodium.h"
#include "Xfbdev_window.h"

/**
 * @class XLinuxMali
 * @brief A concrete implementation of the XPodium class for Linux systems with Mali GPU support.
 * 
 * This class provides specific implementations for managing and interacting with
 * windows on a Linux platform using Mali GPUs. It extends the XPodium base class
 * and provides concrete implementations for window preparation, destruction, and status checking.
 */
class XLinuxMali : public XPodium
{
private:
    static XPodium* instance; ///< Singleton instance of the XLinuxMali class.

    /**
     * @brief Private constructor to enforce the singleton pattern.
     * 
     * The constructor is private to ensure that only one instance of the XLinuxMali
     * class is created. Use the getHandler() method to access the singleton instance.
     */
    XLinuxMali(void);

public:
    /**
     * @brief Retrieves the singleton instance of the XLinuxMali class.
     * 
     * This static method returns the singleton instance of the XLinuxMali class.
     * If the instance does not exist, it will be created.
     * 
     * @return Pointer to the singleton instance of the XLinuxMali class.
     */
    static XPodium* getHandler(void);

    /**
     * @brief Prepares the window with specified dimensions.
     * 
     * This method initializes and configures the window with the given width and height.
     * It overrides the pure virtual function from the XPodium base class.
     * 
     * @param width The width of the window in pixels.
     * @param height The height of the window in pixels.
     */
    virtual void prepareWindow(int width, int height);

    /**
     * @brief Destroys and cleans up the window.
     * 
     * This method releases resources and destroys the window. It overrides the pure
     * virtual function from the XPodium base class.
     */
    virtual void destroyWindow(void);

    /**
     * @brief Checks the current status of the window.
     * 
     * This method checks and returns the current status of the window, such as whether
     * it is idle, requesting to exit, or has detected a mouse click. It overrides the
     * pure virtual function from the XPodium base class.
     * 
     * @return The current status of the window as a WindowStatus enum value.
     */
    virtual WindowStatus checkWindow(void);
};

#endif /* XLINUXTARGET_H */

