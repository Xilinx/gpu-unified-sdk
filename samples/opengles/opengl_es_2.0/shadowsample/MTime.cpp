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
 * @file MTime.cpp
 *
 * @brief This class represents time.
 *
*******************************************************************************/

#include "MTime.h"

MTime::MTime(
    double aSeconds /*= 0.0*/)
    :
    theInitSeconds(aSeconds),
    theSeconds(theInitSeconds),
    //
    theTimer()
  {
  theTimer.reset();
  }

MTime& MTime::operator = (
    const MTime& aOther)
  {
  theSeconds = aOther.theSeconds;
  return *this;
  }

MTime MTime::operator - (
    const MTime& aRight) const
  {
  MTime t(*this);
  t.theSeconds -= aRight.theSeconds;
  return t;
  }

MTime MTime::operator + (
    const MTime& aRight) const
  {
  MTime t(*this);
  t.theSeconds += aRight.theSeconds;
  return t;
  }

void MTime::update()
  {
  theSeconds = theInitSeconds + theTimer.getTime();
  }
