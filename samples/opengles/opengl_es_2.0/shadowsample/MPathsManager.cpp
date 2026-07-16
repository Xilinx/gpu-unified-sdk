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
