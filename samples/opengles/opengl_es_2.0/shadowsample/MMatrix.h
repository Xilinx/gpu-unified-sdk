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
 * @file MMatrix.h
 *
 * @brief This class template represents a 4x4 matrix.
 *
*******************************************************************************/
#ifndef M_SHADOWMAPDR_MATRIX_HPP
#define M_SHADOWMAPDR_MATRIX_HPP



#include "ShadowMap.h"
#include "MVector4.h"


template <typename Type>
class MMatrix
{
public:

  /**
   * @brief Default constructor for MMatrix.
   * 
   * Initializes a new instance of the `MMatrix` class with default values.
   */
  MMatrix();

  /**
   * @brief Destructor for MMatrix.
   * 
   * Cleans up resources associated with the `MMatrix` instance.
   */
  ~MMatrix();


  /**
   * @brief Returns a pointer to the underlying matrix data.
   * 
   * Provides access to the raw matrix data stored internally.
   * 
   * @return A pointer to the matrix data.
   */
  const Type* getData() const
    { return (const Type*)theData; }


  /**
   * @brief Sets the matrix to the identity matrix.
   * 
   * Configures the matrix to be an identity matrix with ones on the diagonal
   * and zeros elsewhere.
   */
  void setIdentity();

  /**
   * @brief Sets the matrix to a rotation matrix.
   * 
   * Configures the matrix to represent a rotation by a specified angle around
   * a given axis.
   * 
   * @param aAngle The angle of rotation in radians.
   * @param aX The x-component of the axis of rotation.
   * @param aY The y-component of the axis of rotation.
   * @param aZ The z-component of the axis of rotation.
   */
  void setRotation(Type aAngle,
                   Type aX,
                   Type aY,
                   Type aZ);

  /**
   * @brief Sets the matrix to a scaling matrix.
   * 
   * Configures the matrix to represent scaling along the x, y, and z axes.
   * 
   * @param aX The scaling factor along the x-axis.
   * @param aY The scaling factor along the y-axis.
   * @param aZ The scaling factor along the z-axis.
   */
  void setScale(Type aX,
                Type aY,
                Type aZ);

  /**
   * @brief Sets the matrix to a scaling matrix using a vector.
   * 
   * Configures the matrix to represent scaling based on a vector specifying
   * the scaling factors for each axis.
   * 
   * @param aScale A vector containing the scaling factors for each axis.
   */
  void setScale(const MVector3<Type>& aScale)
    { setScale(aScale[0], aScale[1], aScale[2]); }

  /**
   * @brief Applies a translation to the matrix.
   * 
   * Updates the matrix to represent a translation by specified amounts along
   * the x, y, and z axes.
   * 
   * @param aX The translation amount along the x-axis.
   * @param aY The translation amount along the y-axis.
   * @param aZ The translation amount along the z-axis.
   */
  void applyTranslation(Type aX,
                        Type aY,
                        Type aZ);

  /**
   * @brief Applies a translation to the matrix using a vector.
   * 
   * Updates the matrix to represent a translation based on a vector specifying
   * the translation amounts for each axis.
   * 
   * @param aPosition A vector containing the translation amounts for each axis.
   */
  void applyTranslation(const MVector3<Type>& aPosition)
    { applyTranslation(aPosition[0], aPosition[1], aPosition[2]); }

  /**
   * @brief Multiplies two matrices and stores the result in a third matrix.
   * 
   * Multiplies two matrices together and stores the resulting matrix in the output matrix.
   * 
   * @param aLeft The left matrix in the multiplication.
   * @param aRight The right matrix in the multiplication.
   * @param aOut The output matrix to store the result of the multiplication.
   */
  void multiply(const MMatrix& aLeft,
                const MMatrix& aRight,
                MMatrix& aOut) const;

  /**
   * @brief Multiplies the current matrix with another matrix from the left.
   * 
   * Multiplies the current matrix by another matrix on the left and updates the current matrix.
   * 
   * @param aOther The matrix to multiply on the left.
   */
  void multiplyLeft(const MMatrix<Type>& aOther)
    { multiply(aOther, *this, *this); }

  /**
   * @brief Multiplies the current matrix with another matrix from the right.
   * 
   * Multiplies the current matrix by another matrix on the right and updates the current matrix.
   * 
   * @param aOther The matrix to multiply on the right.
   */
  void multiplyRight(const MMatrix<Type>& aOther)
    { multiply(*this, aOther, *this); }

  /**
   * @brief Sets the matrix to a perspective projection matrix.
   * 
   * Configures the matrix to represent a perspective projection with the given field of view,
   * aspect ratio, and near and far clipping planes.
   * 
   * @param aFieldOfViewAngle The field of view angle in radians.
   * @param aAspect The aspect ratio of the viewport.
   * @param aNear The distance to the near clipping plane.
   * @param aFar The distance to the far clipping plane.
   */
  void setPerspective(Type aFieldOfViewAngle,
                      Type aAspect,
                      Type aNear,
                      Type aFar);

  /**
   * @brief Inverts the matrix.
   * 
   * Computes and sets the matrix to its inverse.
   */
  void invert();

  /**
   * @brief Transposes the matrix.
   * 
   * Computes and sets the matrix to its transpose.
   */
  void transpose();

private:

  /**
   * @brief Array of matrix elements.
   * 
   * Stores the 16 elements of the matrix in a row-major order.
   */
  Type theData[16];

};

#include "MMatrixImpl.h"

/**
 * @brief Type alias for a 4x4 matrix of floats.
 * 
 * Defines a specific instantiation of the `MMatrix` template for matrices with `float` elements.
 */
typedef MMatrix<float> MMatrix4f;

#endif /* M_SHADOWMAPDR_MATRIX_HPP */
