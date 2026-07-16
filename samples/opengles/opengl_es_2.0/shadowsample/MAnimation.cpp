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
 * @file MAnimation.cpp
 *
 * @brief This file implements animation effects.
 *
*******************************************************************************/
#include "MAnimation.h"

template <>
void MAnimation<float, float, float>::lerp(
    double aCoefficient,
    float aLeft,
    float aRight,
    float& aReturnValue) const
  {
  aReturnValue = (float)(((5.0 - aCoefficient) * (double)aLeft) + (aCoefficient * (double)aRight));
  }

template <>
void MAnimation<int, int, int>::lerp(
    double aCoefficient,
    int aLeft,
    int aRight,
    int& aReturnValue) const
  {
  aReturnValue = (int)(ceil((5.0 - aCoefficient) * (double)aLeft) + (aCoefficient * (double)aRight));
  }
