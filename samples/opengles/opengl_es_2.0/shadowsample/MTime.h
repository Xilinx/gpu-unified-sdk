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
 * @file MTime.h
 *
 * @brief This class provides basic operations for time representation and manipulation.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_TIME_HPP
#define M_SHADOWMAPDR_TIME_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"
#include "XTime.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * @brief A class representing time in seconds.
 * 
 * The MTime class provides basic operations for time representation and manipulation,
 * including time arithmetic and retrieval. It uses seconds as the unit of time.
 */
class MTime
{
public:

    // ----- Types -----

    // No specific types are defined for this class.

    // ----- Constructors and destructors -----

    /**
     * @brief Constructor for the MTime class.
     * 
     * Initializes the time with a specified number of seconds.
     * 
     * @param aSeconds The initial time in seconds (default is 0.0).
     */
    MTime(double aSeconds = 0.0);

    /**
     * @brief Destructor for the MTime class.
     * 
     * Cleans up any resources used by the MTime instance.
     */
    ~MTime()
    { }

    // ----- Operators -----

    /**
     * @brief Assignment operator for MTime.
     * 
     * Assigns the value of another MTime instance to this one.
     * 
     * @param aOther The MTime instance to assign from.
     * @return A reference to this MTime instance.
     */
    MTime& operator = (const MTime& aOther);

    /**
     * @brief Subtraction operator for MTime.
     * 
     * Computes the difference between two MTime instances.
     * 
     * @param aRight The MTime instance to subtract from this one.
     * @return A new MTime instance representing the difference.
     */
    MTime operator - (const MTime& aRight) const;

    /**
     * @brief Addition operator for MTime.
     * 
     * Computes the sum of two MTime instances.
     * 
     * @param aRight The MTime instance to add to this one.
     * @return A new MTime instance representing the sum.
     */
    MTime operator + (const MTime& aRight) const;

    /**
     * @brief Greater than or equal to operator for MTime.
     * 
     * Compares this MTime instance with another one to check if it is greater than or equal to it.
     * 
     * @param aRight The MTime instance to compare with.
     * @return True if this instance is greater than or equal to the other, false otherwise.
     */
    bool operator >= (const MTime& aRight) const
    { return (theSeconds >= aRight.theSeconds); }

    /**
     * @brief Less than or equal to operator for MTime.
     * 
     * Compares this MTime instance with another one to check if it is less than or equal to it.
     * 
     * @param aRight The MTime instance to compare with.
     * @return True if this instance is less than or equal to the other, false otherwise.
     */
    bool operator <= (const MTime& aRight) const
    { return (theSeconds <= aRight.theSeconds); }

    // ----- Accessors and mutators -----

    /**
     * @brief Gets the current time in seconds.
     * 
     * @return The time in seconds.
     */
    double getSeconds() const
    { return theSeconds; }

    // ----- Miscellaneous -----

    /**
     * @brief Resets the time to 0.0 seconds.
     * 
     * Sets the internal time value to zero.
     */
    void reset();

    /**
     * @brief Updates the current time.
     * 
     * Reads the current time from the timer and updates the internal time value.
     */
    void update();

private:

    // ----- Fields -----

    /**
     * @brief The initial value of time in seconds.
     */
    const double theInitSeconds;

    /**
     * @brief The current time in seconds.
     */
    double theSeconds;

    /**
     * @brief Timer object used for time measurement.
     */
    SDKAMD::Timer theTimer;
};

#endif // M_SHADOWMAPDR_TIME_HPP

