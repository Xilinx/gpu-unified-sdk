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
 * @file MPathsManager.cpp
 *
 * @brief This file implements all the functions related to path management.
 *
*******************************************************************************/

#include "MPathsManager.h"

MPathsManager& MPathsManager::getInstance()
  {
  static MPathsManager mgr;
  return mgr;
  }

MPathsManager::MPathsManager()
    :
    theJNIEnv(NULL),
    theAppId("")
  {
  }

MPathsManager::~MPathsManager()
  {
  theJNIEnv = NULL;
  theAppId = "";
  }

MPath MPathsManager::getFullPathStatic(
    const MPath& aFile)
  {
  return getInstance().getFullPath(aFile);
  }

bool MPathsManager::initializeStatic(
    JNIEnv* aEnv/* = NULL*/,
    const MString& aAppIdentifier/* = ""*/)
  {
  return getInstance().initialize(aEnv, aAppIdentifier);
  }

bool MPathsManager::initialize(
    JNIEnv* aEnv/* = NULL*/,
    const MString& aAppIdentifier/* = ""*/)
  {
  theJNIEnv = aEnv;
  theAppId =  aAppIdentifier;
  return true;
  }

MPath MPathsManager::getFullPath(
    const MPath& aFile)
  {
#if defined(_WIN32) || defined(__gnu_linux__)
    MPath r = "Supportingfiles/";
    r += aFile;
    return r;
#elif defined(__android__)
  MPath resFileDir("/data/data/");
  resFileDir += theAppId;
  resFileDir += "/";
  //
  if (!aFile.getLength())
    return resFileDir;
  //
  MPath resFilePath(resFileDir);
  resFilePath += aFile;
  //
  if (theJNIEnv == NULL)
    {
    LOGE("[MPathsManager::getFullPath(\"%s\")] The managers is not initialized!\n", aFile.getData());
    }
  getAndroidAsset(theJNIEnv, (char*)resFileDir.getData(), (char*)resFilePath.getData(), (char*)aFile.getData());
  //
  return resFilePath;
#else
    return "NO_PLATFORM";
#endif
  }
