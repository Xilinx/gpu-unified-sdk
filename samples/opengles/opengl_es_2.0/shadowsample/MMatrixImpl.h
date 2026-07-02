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
 * @file MMatrixImpl.h
 *
 * @brief This class is the implementation of the matrix.
 *
*******************************************************************************/

#ifndef M_SHADOWMAPDR_MATRIX_IMPL_HPP
#define M_SHADOWMAPDR_MATRIX_IMPL_HPP

#include "XMathematics.h"

// ----- MMatrix Implementation -----

template <typename Type>
MMatrix<Type>::MMatrix()
{
  setIdentity();
}

template <typename Type>
MMatrix<Type>::~MMatrix()
{
}

template <typename Type>
void MMatrix<Type>::setIdentity()
{
  // Initialize the matrix to the identity matrix
  memset(theData, 0, 16 * sizeof(Type));
  theData[0]  = 1;
  theData[5]  = 1;
  theData[10] = 1;
  theData[15] = 1;
}

template <typename Type>
void MMatrix<Type>::setRotation(
    Type aAngle,
    Type aX,
    Type aY,
    Type aZ)
{
  // Initialize rotation variables
  double radians, c, s, c1, u[3], length;
  int i, j;

  // Convert angle from degrees to radians
  radians = (aAngle * M_PI) / 180.0;

  c = cos(radians);  // Cosine of angle
  s = sin(radians);  // Sine of angle

  c1 = 1.0 - c;  // 1 - cosine

  // Normalize the axis of rotation
  length = sqrt(aX * aX + aY * aY + aZ * aZ);
  u[0] = aX / length;
  u[1] = aY / length;
  u[2] = aZ / length;

  // Initialize matrix to identity
  for (i = 0; i < 16; i++)
  {
    theData[i] = 0.0;
  }
  theData[15] = 1.0;

  // Populate rotation matrix elements
  for (i = 0; i < 3; i++)
  {
    theData[i * 4 + (i + 1) % 3] = (Type) (u[(i + 2) % 3] * s);
    theData[i * 4 + (i + 2) % 3] = (Type) (-u[(i + 1) % 3] * s);
  }

  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 3; j++)
    {
      theData[i * 4 + j] += (Type) (c1 * u[i] * u[j] + (i == j ? c : 0.0));
    }
  }
}

template <typename Type>
void MMatrix<Type>::setScale(
    Type aX,
    Type aY,
    Type aZ)
{
  setIdentity();
  // Set scaling factors on diagonal
  theData[0] = aX;
  theData[5] = aY;
  theData[10] = aZ;
}

template <typename Type>
void MMatrix<Type>::applyTranslation(
    Type aX,
    Type aY,
    Type aZ)
{
  // Apply translation to the matrix
  theData[12] += aX;
  theData[13] += aY;
  theData[14] += aZ;
}

template <typename Type>
void MMatrix<Type>::multiply(
    const MMatrix<Type>& aLeft,
    const MMatrix<Type>& aRight,
    MMatrix<Type>& aOut) const
{
  const Type *A = (const Type*)aLeft.theData;
  const Type *B = (const Type*)aRight.theData;
  Type *C = (Type*)aOut.theData;

  int i, j, k;
  Type aTmp[16];

  // Perform matrix multiplication
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      aTmp[j * 4 + i] = 0.0;
      for (k = 0; k < 4; k++)
      {
        aTmp[j * 4 + i] += A[k * 4 + i] * B[j * 4 + k];
      }
    }
  }

  // Copy result to output matrix
  for (i = 0; i < 16; i++)
  {
    C[i] = aTmp[i];
  }
}

template <typename Type>
void MMatrix<Type>::setPerspective(
    Type aFieldOfViewAngle,
    Type aAspectRatio,
    Type aNear,
    Type aFar)
{
  setIdentity();
  Type* P = (Type*)theData;

  // Calculate perspective matrix values
  double f = 1.0 / tan(aFieldOfViewAngle * 0.5);

  P[0] = (Type) (f / aAspectRatio);
  P[5] = (Type) (f);
  P[10] = (Type) ((aNear + aFar) / (aNear - aFar));
  P[11] = -1.0f;
  P[14] = (Type) ((2.0 * aNear * aFar) / (aNear - aFar));
  P[15] = 0.0f;
}

template <typename Type>
void MMatrix<Type>::invert()
{
  // Use external library to invert the matrix
  XMatrixAPI* mat = (XMatrixAPI*)theData;
  XMatrixAPI matInv = XMatrixAPI::XMatrixAPIInvert(mat);
  memcpy(theData, &matInv, sizeof(XMatrixAPI));
}

template <typename Type>
void MMatrix<Type>::transpose()
{
  // Perform matrix transposition
  float fTemp;

  fTemp = theData[1];
  theData[1] = theData[4];
  theData[4] = fTemp;

  fTemp = theData[2];
  theData[2] = theData[8];
  theData[8] = fTemp;

  fTemp = theData[3];
  theData[3] = theData[12];
  theData[12] = fTemp;

  fTemp = theData[6];
  theData[6] = theData[9];
  theData[9] = fTemp;

  fTemp = theData[7];
  theData[7] = theData[13];
  theData[13] = fTemp;

  fTemp = theData[11];
  theData[11] = theData[14];
  theData[14] = fTemp;
}

#endif /* M_SHADOWMAPDR_MATRIX_IMPL_HPP */