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
 * @file XMathematics.h
 *
 * This file implements all the functions related to math operations.
 *
********************************************************************************/

#ifndef XMATHOPERATIONS_H
#define XMATHOPERATIONS_H

#include "XCVector.h"
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846f ///< Definition of PI if not already defined.
#endif 

/**
 * @brief Computes the distance between two 2D points.
 * 
 * This function calculates the Euclidean distance between two points in a 2D space.
 * 
 * @param point1 The first point as a CVec2f object.
 * @param point2 The second point as a CVec2f object.
 * @return The distance between the two points.
 */
inline float distanceBetweenPoints(const CVec2f& point1, const CVec2f& point2)
{
    return sqrtf((point2.x - point1.x) * (point2.x - point1.x) + (point2.y - point1.y) * (point2.y - point1.y));
}

/**
 * @brief Converts an angle from degrees to radians.
 * 
 * This function converts an angle measured in degrees to its equivalent in radians.
 * 
 * @param degrees The angle in degrees.
 * @return The angle in radians.
 */
inline float degreesToRadians(float degrees)
{
    return M_PI * degrees / 180.0f;
}

/**
 * @brief Returns the sign of a floating-point number.
 * 
 * This function returns 1.0 if the input number is positive, -1.0 if it is negative, 
 * and 0.0 if the number is zero.
 * 
 * @param f The floating-point number to determine the sign of.
 * @return The sign of the input number.
 */
inline float signum(float f)
{
    if (f > 0.0f) 
    {
        return 1.0f;
    }
    
    if (f < 0.0f) 
    {
        return -1.0f;
    }

    return 0.0f;
}

/**
 * @brief Generates a uniform random number between 0 and 1.
 * 
 * This function generates a random floating-point number between 0 and 1, inclusive.
 * 
 * @return A random floating-point number between 0 and 1.
 */
inline float uniformRandomNumber()
{
    return rand() / float(RAND_MAX);
}

#endif /* XMATHOPERATIONS_H */