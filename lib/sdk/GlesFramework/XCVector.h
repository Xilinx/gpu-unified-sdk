/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/******************************************************************************/
/**
 *
 * @file Xvector.h
 *
 * This file implements all basic Vector types
 *
********************************************************************************/
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
#ifndef XVECTOR_H
#define XVECTOR_H

#include <cmath>

/**
 * @class CVec2
 * @brief Represents a 2D vector with integer components.
 * 
 * This class defines a 2D vector with integer components `x` and `y`.
 */
class CVec2
{
public:
    int x; ///< The x component of the vector.
    int y; ///< The y component of the vector.
};

/**
 * @class CVec3
 * @brief Represents a 3D vector with integer components.
 * 
 * This class defines a 3D vector with integer components `x`, `y`, and `z`.
 */
class CVec3
{
public:
    int x; ///< The x component of the vector.
    int y; ///< The y component of the vector.
    int z; ///< The z component of the vector.
};

/**
 * @class CVec4
 * @brief Represents a 4D vector with integer components.
 * 
 * This class defines a 4D vector with integer components `x`, `y`, `z`, and `w`.
 */
class CVec4
{
public:
    int x; ///< The x component of the vector.
    int y; ///< The y component of the vector.
    int z; ///< The z component of the vector.
    int w; ///< The w component of the vector.
};

/**
 * @class CVec2f
 * @brief Represents a 2D vector with floating-point components.
 * 
 * This class defines a 2D vector with floating-point components `x` and `y`.
 */
class CVec2f
{
public:
    float x; ///< The x component of the vector.
    float y; ///< The y component of the vector.
};

/**
 * @class CVec3f
 * @brief Represents a 3D vector with floating-point components.
 * 
 * This class defines a 3D vector with floating-point components `x`, `y`, and `z`.
 */
class CVec3f
{
public:
    float x; ///< The x component of the vector.
    float y; ///< The y component of the vector.
    float z; ///< The z component of the vector.

    /**
     * @brief Normalizes the vector to unit length.
     * 
     * This method scales the vector such that its length becomes 1.0. It modifies
     * the vector in place.
     */
    void normalize(void)
    {
        float length = sqrt(x * x + y * y + z * z);

        x /= length;
        y /= length;
        z /= length;
    }

    /**
     * @brief Computes the cross product of two 3D vectors.
     * 
     * This static method calculates the cross product of two CVec3f vectors and returns
     * the result as a new CVec3f vector.
     * 
     * @param vector1 The first vector in the cross product operation.
     * @param vector2 The second vector in the cross product operation.
     * @return The cross product of vector1 and vector2.
     */
    static CVec3f cross(const CVec3f& vector1, const CVec3f& vector2)
    {
        CVec3f crossProduct;

        crossProduct.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
        crossProduct.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
        crossProduct.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);

        return crossProduct;
    }

    /**
     * @brief Computes the dot product of two 3D vectors.
     * 
     * This static method calculates the dot product of two CVec3f vectors.
     * 
     * @param vector1 The first vector in the dot product operation.
     * @param vector2 The second vector in the dot product operation.
     * @return The dot product of vector1 and vector2.
     */
    static float dot(CVec3f& vector1, CVec3f& vector2)
    {
        return (vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z);
    }
};

/**
 * @class CVec4f
 * @brief Represents a 4D vector with floating-point components.
 * 
 * This class defines a 4D vector with floating-point components `x`, `y`, `z`, and `w`.
 */
class CVec4f
{
public:
    float x; ///< The x component of the vector.
    float y; ///< The y component of the vector.
    float z; ///< The z component of the vector.
    float w; ///< The w component of the vector.

    /**
     * @brief Normalizes the vector to unit length.
     * 
     * This method scales the vector such that its length becomes 1.0. It modifies
     * the vector in place.
     */
    void normalize(void)
    {
        float length = sqrt(x * x + y * y + z * z + w * w);

        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }
};

#endif /* XVECTOR_H */
