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
 * @file MPathsManager.h
 *
 * @brief  This is a global object which we use to translate resource names from PC format into Android format (if we're running on Android). It also
 * pulls things out of the bundle when requested, meaning we no longer
 * need a separate section of code which does all that work.
 * 
 * If you're on android, you'll need to configure things like the project
 * name and the JNIenv data pointer and so on.
 *
*******************************************************************************/
#ifndef _M_PATHS_MANAGER_HPP
#define _M_PATHS_MANAGER_HPP

#include "ShadowMap.h"
#include "MString.h"

#ifdef __android__
# include <jni.h>
#endif

/**
 * @brief Manages file paths for different platforms.
 * 
 * The MPathsManager class provides functionality to retrieve full paths for asset files. 
 * It handles platform-specific details, including JNI environment management for Android.
 */
class MPathsManager
{
public:

  // ----- Types -----

#ifndef __android__
  /// Definition for JNIEnv when not on Android platform. It is a placeholder type.
  typedef void JNIEnv;
#endif

  // ----- Statics -----

  /**
   * @brief Retrieves the singleton instance of MPathsManager.
   * 
   * This static method returns the single instance of MPathsManager, ensuring that only one 
   * instance exists throughout the application. It follows the Singleton design pattern.
   * 
   * @return A reference to the singleton instance of MPathsManager.
   */
  static MPathsManager& getInstance();

  // ----- Miscellaneous -----

  /**
   * @brief Static method to initialize the MPathsManager.
   * 
   * Initializes the singleton instance of MPathsManager with the given JNI environment and 
   * application identifier. This method must be called before using other static methods.
   * 
   * @param aEnv Pointer to the JNI environment for Android platform. Default is NULL.
   * @param aAppIdentifier Application identifier for Android platform. Default is an empty string.
   * 
   * @return True if initialization is successful, false otherwise.
   */
  static bool initializeStatic(JNIEnv* aEnv = NULL,
                               const MString& aAppIdentifier = "");

  /**
   * @brief Static method to get the full path of a file.
   * 
   * Retrieves the full path for the specified file by using the singleton instance of 
   * MPathsManager.
   * 
   * @param aFile The relative path of the file.
   * 
   * @return The full path of the file as an MPath object.
   */
  static MPath getFullPathStatic(const MPath& aFile);

private:

  // ----- Fields -----

  /// Pointer to the JNI environment, used only on Android platform.
  JNIEnv* theJNIEnv;

  /// Application identifier for Android platform.
  MString theAppId;

  // ----- Constructors and destructors -----

  /**
   * @brief Default constructor for MPathsManager.
   * 
   * Initializes the MPathsManager with default values.
   */
  MPathsManager();

  /**
   * @brief Destructor for MPathsManager.
   * 
   * Cleans up resources and resets member variables to default values.
   */
  ~MPathsManager();

  // ----- Miscellaneous -----

  /**
   * @brief Initializes the MPathsManager instance.
   * 
   * Sets the JNI environment and application identifier for the instance. This method should 
   * be called before using other methods.
   * 
   * @param aEnv Pointer to the JNI environment. Default is NULL.
   * @param aAppIdentifier Application identifier for Android platform. Default is an empty string.
   * 
   * @return True if initialization is successful, false otherwise.
   */
  bool initialize(JNIEnv* aEnv = NULL,
                  const MString& aAppIdentifier = "");

  /**
   * @brief Retrieves the full path for a given file.
   * 
   * Constructs the full path for the specified file based on the platform. Handles path 
   * construction for different operating systems including Windows, Linux, and Android.
   * 
   * @param aFile The relative path of the file.
   * 
   * @return The full path of the file as an MPath object.
   */
  MPath getFullPath(const MPath& aFile);

  // ----- Prevent copying and assignment -----

  /**
   * @brief Copy constructor is deleted to prevent copying.
   * 
   * Copy construction is not allowed for MPathsManager.
   */
  MPathsManager(const MPathsManager &other);

  /**
   * @brief Assignment operator is deleted to prevent assignment.
   * 
   * Assignment is not allowed for MPathsManager.
   */
  MPathsManager &operator=(const MPathsManager &other);
};

#endif