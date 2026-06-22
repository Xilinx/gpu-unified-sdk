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
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 * @endcond
 *
*******************************************************************************/
/**
 *
 * @file XMatrixAPI.h
 *
 * @brief This file implements all the functions related to Matrix calculations for application.
 *
*******************************************************************************/
#ifndef MATRIX_H
#define MATRIX_H

#include "XCVector.h"

/**
 * @class XMatrixAPI
 * @brief A class for representing and manipulating 4x4 matrices.
 * 
 * This class provides various methods for creating, manipulating, and transforming
 * 4x4 matrices used in graphics applications. It supports matrix multiplication, 
 * rotation, translation, scaling, and perspective projection.
 */
class XMatrixAPI
{
private:
    float elements[16]; ///< Array storing the elements of the 4x4 matrix.

    /**
     * @brief Multiplies two matrices.
     * 
     * This static method multiplies two matrices and returns the result.
     * 
     * @param left Pointer to the left matrix.
     * @param right Pointer to the right matrix.
     * @return The result of multiplying the left and right matrices.
     */
    static XMatrixAPI multiply(XMatrixAPI *left, XMatrixAPI *right);

    static const float identityArray[]; ///< Array representing the identity matrix.

public:
	/**
     * @brief Gets the matrix elements as an array.
     * 
     * @return Pointer to an array of 16 float values representing the matrix.
     */
    float* getAsArray(void);

    /**
     * @brief Default constructor.
     * 
     * Initializes a new instance of the XMatrixAPI class with default values.
     */
    XMatrixAPI(void);

    /**
     * @brief Constructor that initializes the matrix with values from an array.
     * 
     * @param array Pointer to an array of 16 float values used to initialize the matrix.
     */
    XMatrixAPI(const float* array);

    /**
     * @brief Accesses a matrix element by index.
     * 
     * Provides access to matrix elements using the array subscript operator.
     * 
     * @param element The index of the element to access (0 to 15).
     * @return Reference to the matrix element at the specified index.
     */
    float& operator[] (unsigned element);

    /**
     * @brief Multiplies the current matrix by another matrix.
     * 
     * @param right The matrix to multiply with the current matrix.
     * @return The result of the multiplication.
     */
	 
	 static void XMatrixAPITranspose(XMatrixAPI *XMatrixAPI);
	 
	 static CVec4f vertexTransform(CVec4f *vector, XMatrixAPI *XMatrixAPI);
     static CVec3f vertexTransform(CVec3f *vector, XMatrixAPI *XMatrixAPI);
	 
    XMatrixAPI operator* (XMatrixAPI right);

    /**
     * @brief Assignment operator.
     * 
     * Copies the contents of another XMatrixAPI instance to the current instance.
     * 
     * @param another The matrix to copy from.
     * @return Reference to the current instance.
     */
    XMatrixAPI& operator=(const XMatrixAPI &another);
	
	static XMatrixAPI identityXMatrixAPI;
	

    /**
     * @brief Creates a rotation matrix around the X-axis.
     * 
     * @param angle The angle of rotation in radians.
     * @return A matrix representing the rotation around the X-axis.
     */
    static XMatrixAPI createRotationX(float angle);
    /**
     * @brief Creates a rotation matrix around the Y-axis.
     * 
     * @param angle The angle of rotation in radians.
     * @return A matrix representing the rotation around the Y-axis.
     */
    static XMatrixAPI createRotationY(float angle);

    /**
     * @brief Creates a rotation matrix around the Z-axis.
     * 
     * @param angle The angle of rotation in radians.
     * @return A matrix representing the rotation around the Z-axis.
     */
    static XMatrixAPI createRotationZ(float angle);

    /**
     * @brief Creates a translation matrix.
     * 
     * @param x The translation distance along the X-axis.
     * @param y The translation distance along the Y-axis.
     * @param z The translation distance along the Z-axis.
     * @return A matrix representing the translation.
     */
    static XMatrixAPI createTranslation(float x, float y, float z);

    /**
     * @brief Creates a scaling matrix.
     * 
     * @param x The scaling factor along the X-axis.
     * @param y The scaling factor along the Y-axis.
     * @param z The scaling factor along the Z-axis.
     * @return A matrix representing the scaling.
     */
    static XMatrixAPI createScaling(float x, float y, float z);

    /**
     * @brief Prints the matrix elements to the standard output.
     * 
     * Outputs the matrix in a human-readable format.
     */
    void print(void);

    /**
     * @brief Creates a perspective projection matrix.
     * 
     * @param FOV The field of view angle in radians.
     * @param ratio The aspect ratio (width/height) of the viewport.
     * @param zNear The distance to the near clipping plane.
     * @param zFar The distance to the far clipping plane.
     * @return A matrix representing the perspective projection.
     */
    static XMatrixAPI XMatrixAPIPerspective(float FOV, float ratio, float zNear, float zFar);

    /**
     * @brief Creates a camera view matrix based on the look-at parameters.
     * 
     * @param eye The position of the camera.
     * @param center The point the camera is looking at.
     * @param up The up direction of the camera.
     * @return A matrix representing the camera view transformation.
     */
    static XMatrixAPI XMatrixAPICameraLookAt(CVec3f eye, CVec3f center, CVec3f up);

    /**
     * @brief Creates an orthographic projection matrix.
     * 
     * @param left The coordinate for the left side of the viewing volume.
     * @param right The coordinate for the right side of the viewing volume.
     * @param bottom The coordinate for the bottom of the viewing volume.
     * @param top The coordinate for the top of the viewing volume.
     * @param zNear The distance to the near clipping plane.
     * @param zFar The distance to the far clipping plane.
     * @return A matrix representing the orthographic projection.
     */
    static XMatrixAPI XMatrixAPIOrthographic(float left, float right, float bottom, float top, float zNear, float zFar);

    /**
     * @brief Computes the inverse of a matrix.
     * 
     * @param XMatrixAPI Pointer to the matrix to be inverted.
     * @return The inverse of the provided matrix.
     */
    static XMatrixAPI XMatrixAPIInvert(XMatrixAPI *XMatrixAPI);

    /**
     * @brief Computes the determinant of a matrix.
     * 
     * @param XMatrixAPI Pointer to the matrix for which the determinant is computed.
     * @return The determinant of the provided matrix.
     */
    static float XMatrixAPIDeterminant(float *XMatrixAPI);

    /**
     * @brief Computes the determinant of a matrix.
     * 
     * @param XMatrixAPI Pointer to the matrix for which the determinant is computed.
     * @return The determinant of the provided matrix.
     */
    static float XMatrixAPIDeterminant(XMatrixAPI *XMatrixAPI);

    /**
     * @brief Scales the matrix by a given factor.
     * 
     * @param XMatrixAPI Pointer to the matrix to be scaled.
     * @param scale The scaling factor.
     * @return The scaled matrix.
     */
    static XMatrixAPI XMatrixAPIScale(XMatrixAPI *XMatrixAPI, float scale);
};

#endif /* MATRIX_H */