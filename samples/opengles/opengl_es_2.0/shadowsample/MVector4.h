/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */

#ifndef M_SHADOWMAP_VECTOR4_HPP
#define M_SHADOWMAP_VECTOR4_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"

/**
 * @brief A template class representing a 4-dimensional vector.
 * @tparam Type The type of the vector elements (e.g., float, double, int).
 */
template <typename Type>
class MVector4
{
public:

  /**
   * @brief Default constructor. Initializes the vector to (0, 0, 0, 0).
   */
  MVector4();

  /**
   * @brief Parameterized constructor. Initializes the vector with the given values.
   * @param a1 The first component of the vector.
   * @param a2 The second component of the vector.
   * @param a3 The third component of the vector.
   * @param a4 The fourth component of the vector.
   */
  MVector4(const Type& a1,
           const Type& a2,
           const Type& a3,
           const Type& a4)
    {
    set(a1, a2, a3, a4);
    }

  /**
   * @brief Destructor.
   */
  ~MVector4();

  // ----- Operators -----

  /**
   * @brief Subtracts another vector from this vector and updates it.
   * @param aRight The vector to subtract.
   * @return Reference to this vector.
   */
 MVector4<Type>& operator-(const MVector4<Type>& aRight)
    {
    translate(-aRight[0], -aRight[1], -aRight[2], -aRight[3]);
    return *this;
    }
  /**
   * @brief Adds another vector to this vector.
   * @param aRight The vector to add.
   * @return Reference to this vector.
   */
   MVector4<Type>& operator+=(const MVector4<Type>& aRight)
    {
    translate(aRight);
    return *this;
    }

  /**
   * @brief Subtracts another vector from this vector and updates it.
   * @param aRight The vector to subtract.
   * @return Reference to this vector.
   */
  MVector4<Type>& operator-=(const MVector4<Type>& aRight)
    {
    translate(-aRight[0], -aRight[1], -aRight[2], -aRight[3]);
    return *this;
    }

  /**
   * @brief Accesses the component at the specified index.
   * @param aIndex The index of the component.
   * @return Reference to the component at the specified index.
   */
    Type& operator[] (unsigned int aIndex)
    { return theV[aIndex]; }
  /**
   * @brief Accesses the component at the specified index (const version).
   * @param aIndex The index of the component.
   * @return Constant reference to the component at the specified index.
   */
   const Type& operator[] (unsigned int aIndex) const
    { return theV[aIndex]; }
  // ----- Accessors and mutators -----

  /**
   * @brief Returns a pointer to the vector data.
   * @return Pointer to the vector data.
   */
    const Type* getData() const
    { return theV; }
  // ----- Miscellaneous -----

  /**
   * @brief Sets the components of the vector.
   * @param a1 The first component.
   * @param a2 The second component.
   * @param a3 The third component.
   * @param a4 The fourth component.
   */
  void set(const Type& a1,
           const Type& a2,
           const Type& a3,
           const Type& a4);

  /**
   * @brief Translates the vector by adding the components of another vector.
   * @param aRight The vector to add.
   */
    void translate(const MVector4<Type>& aRight)
    { translate(aRight[0], aRight[1], aRight[2], aRight[3]); }

  /**
   * @brief Translates the vector by adding the specified values.
   * @param a1 The value to add to the first component.
   * @param a2 The value to add to the second component.
   * @param a3 The value to add to the third component.
   * @param a4 The value to add to the fourth component.
   */
  void translate(const Type& a1,
                 const Type& a2,
                 const Type& a3,
                 const Type& a4);

private:

  // ----- Fields -----

  /**
   * @brief The vector components.
   */
  Type theV[4];

};

// Template method implementations

template <typename Type>
MVector4<Type>::MVector4()
{
  memset(theV, 0, sizeof(Type) * 4);
}

template <typename Type>
MVector4<Type>::~MVector4()
{
}

template <typename Type>
void MVector4<Type>::set(
    const Type& a1,
    const Type& a2,
    const Type& a3,
    const Type& a4)
{
  theV[0] = a1;
  theV[1] = a2;
  theV[2] = a3;
  theV[3] = a4;
}

template <typename Type>
void MVector4<Type>::translate(
    const Type& a1,
    const Type& a2,
    const Type& a3,
    const Type& a4)
{
  theV[0] += a1;
  theV[1] += a2;
  theV[2] += a3;
  theV[3] += a4;
}

// Type definitions for specific types
typedef MVector4<float> MVector4f;
typedef MVector4<double> MVector4d;
typedef MVector4<int> MVector4i;
typedef MVector4<unsigned int> MVector4ui;

// BEGIN OF CLASS DECLARATION

/**
 * @brief A template class representing a 2-dimensional vector.
 * @tparam Type The type of the vector components (e.g., float, double, int).
 */
template <typename Type>
class MVector2
{
public:

  // ----- Constructors and destructors -----

  /**
   * @brief Default constructor. Initializes the vector to (0, 0).
   */
  MVector2();

  /**
   * @brief Parameterized constructor. Initializes the vector with the given values.
   * @param a1 The first component of the vector.
   * @param a2 The second component of the vector.
   */
  MVector2(const Type& a1,
           const Type& a2)
    {
    set(a1, a2);
    }

  /**
   * @brief Destructor.
   */
  ~MVector2();

  // ----- Operators -----

  /**
   * @brief Adds another vector to this vector.
   * @param aRight The vector to add.
   * @return Reference to this vector after addition.
   */
  MVector2<Type>& operator+=(const MVector2<Type>& aRight)
  {
    translate(aRight);
    return *this;
    }

  /**
   * @brief Subtracts another vector from this vector.
   * @param aRight The vector to subtract.
   * @return Reference to this vector after subtraction.
   */
  MVector2<Type>& operator-=(const MVector2<Type>& aRight)
    {
    translate(-aRight.theV[0], -aRight.theV[1]);
    return *this;
    }

  /**
   * @brief Multiplies this vector element-wise by another vector.
   * @param aRight The vector to multiply by.
   * @return Reference to this vector after multiplication.
   */
  MVector2<Type>& operator*=(const MVector2<Type>& aRight)
    {
    theV[0] *= aRight.theV[0];
    theV[1] *= aRight.theV[1];
    return *this;
    }
  /**
   * @brief Multiplies this vector by a scalar value.
   * @param aValue The scalar value to multiply by.
   * @return Reference to this vector after multiplication.
   */
    MVector2<Type>& operator*=(Type aValue)
    {
    theV[0] *= aValue;
    theV[1] *= aValue;
    return *this;
    }

  /**
   * @brief Subtracts another vector from this vector and returns the result.
   * @param aRight The vector to subtract.
   * @return A new vector resulting from the subtraction.
   */
  MVector2 operator-(const MVector2& aRight) const
    {
    MVector2 tmp(*this);
    tmp.theV[0] -= aRight.theV[0];
    tmp.theV[1] -= aRight.theV[1];
    return tmp;
    }

  /**
   * @brief Multiplies this vector by a scalar value and returns the result.
   * @param aValue The scalar value to multiply by.
   * @return A new vector resulting from the multiplication.
   */
    MVector2 operator*(Type aValue) const
    {
    MVector2 tmp(*this);
    tmp.theV[0] *= aValue;
    tmp.theV[1] *= aValue;
    return tmp;
    }

  /**
   * @brief Accesses the component at the specified index.
   * @param aIndex The index of the component (0 or 1).
   * @return Reference to the component at the specified index.
   */
    Type& operator[] (unsigned int aIndex)
    { return theV[aIndex]; }

  /**
   * @brief Accesses the component at the specified index (const version).
   * @param aIndex The index of the component (0 or 1).
   * @return Constant reference to the component at the specified index.
   */
  const Type& operator[] (unsigned int aIndex) const
    { return theV[aIndex]; }

  // ----- Accessors and mutators -----

  /**
   * @brief Returns a pointer to the vector data.
   * @return Pointer to the vector data.
   */
   const Type* getData() const
    { return &theV; }
  // ----- Miscellaneous -----

  /**
   * @brief Sets the components of the vector.
   * @param a1 The first component.
   * @param a2 The second component.
   */
  void set(Type a1,
           Type a2);

  /**
   * @brief Translates (adds) the components of another vector to this vector.
   * @param aRight The vector to add.
   */
  void translate(const MVector2<Type>& aRight);

  /**
   * @brief Translates (adds) the specified values to the components of this vector.
   * @param a1 The value to add to the first component.
   * @param a2 The value to add to the second component.
   */
  void translate(Type a1,
                 Type a2);

  /**
   * @brief Computes the length (magnitude) of the vector.
   * @return The length of the vector.
   */
  Type length() const;

private:

  // ----- Fields -----

  /**
   * @brief The vector components.
   */
  Type theV[2];

};

// Template method implementations

template <typename Type>
MVector2<Type>::MVector2()
{
  memset(theV, 0, sizeof(Type) * 2);
}

template <typename Type>
MVector2<Type>::~MVector2()
{
}

template <typename Type>
void MVector2<Type>::set(
    Type a1,
    Type a2)
{
  theV[0] = a1;
  theV[1] = a2;
}

template <typename Type>
void MVector2<Type>::translate(
    const MVector2<Type>& aRight)
{
  theV[0] += aRight[0];
  theV[1] += aRight[1];
}

template <typename Type>
void MVector2<Type>::translate(
    Type a1,
    Type a2)
{
  theV[0] += a1;
  theV[1] += a2;
}

template <typename Type>
Type MVector2<Type>::length() const
{
  return (Type)sqrt((double)(theV[0] * theV[0] +
                             theV[1] * theV[1]));
}

// Type definitions for specific types
typedef MVector2<float> MVector2f;
typedef MVector2<double> MVector2d;
typedef MVector2<int> MVector2i;
typedef MVector2<unsigned int> MVector2ui;


/**
 * @brief A template class representing a 3-dimensional vector.
 * @tparam Type The type of the vector components (e.g., float, double, int).
 * @tparam PassType The type used for passing values to the class methods (defaults to Type).
 */
template <typename Type, typename PassType=Type>
class MVector3
  {
public:
/**
   * @brief Default constructor. Initializes the vector to (0, 0, 0).
   */
  MVector3();

  /**
   * @brief Parameterized constructor. Initializes the vector with the given values.
   * @param a1 The first component of the vector.
   * @param a2 The second component of the vector.
   * @param a3 The third component of the vector.
   */
  MVector3(const PassType& a1,
           const PassType& a2,
           const PassType& a3)
    {
    set(a1, a2, a3);
    }

  /**
   * @brief Destructor.
   */
  ~MVector3();

  /**
   * @brief Adds a scalar value to each component of the vector and returns the result.
   * @param aRightValue The scalar value to add.
   * @return A new vector resulting from the addition.
   */
  MVector3 operator+(PassType aRightValue) const
    {
    MVector3 tmp(*this);
    tmp.translate(aRightValue, aRightValue, aRightValue);
    return tmp;
    }

  /**
   * @brief Multiplies each component of the vector by a scalar value and returns the result.
   * @param aRightValue The scalar value to multiply by.
   * @return A new vector resulting from the multiplication.
   */  
  MVector3 operator*(PassType aRightValue) const
    {
    MVector3 tmp(*this);
    tmp[0] *= aRightValue;
    tmp[1] *= aRightValue;
    tmp[2] *= aRightValue;
    return tmp;
    }

  /**
   * @brief Adds another vector to this vector and returns the result.
   * @param aRight The vector to add.
   * @return A new vector resulting from the addition.
   */
  MVector3 operator+(const MVector3& aRight) const
    {
    MVector3 tmp(*this);
    tmp.translate(aRight[0], aRight[1], aRight[2]);
    return tmp;
    }

  /**
   * @brief Subtracts another vector from this vector and returns the result.
   * @param aRight The vector to subtract.
   * @return A new vector resulting from the subtraction.
   */
  MVector3 operator-(const MVector3& aRight) const
    {
    MVector3 tmp(*this);
    tmp.translate(-aRight[0], -aRight[1], -aRight[2]);
    return tmp;
    }

  /**
   * @brief Divides this vector by another vector element-wise and returns the result.
   * @param aRight The vector to divide by.
   * @return A new vector resulting from the division.
   */
  MVector3 operator/(const MVector3& aRight) const
    {
    MVector3 tmp(theV[0] / aRight[0],
                   theV[1] / aRight[1],
                   theV[2] / aRight[2]);
    return tmp;
    }

  /**
   * @brief Subtracts another vector from this vector.
   * @param aRight The vector to subtract.
   * @return Reference to this vector after subtraction.
   */
  MVector3& operator-(const MVector3& aRight)
    {
    MVector3 tmp(*this);
    tmp.translate(-aRight[0], -aRight[1], -aRight[2]);
    return tmp;
    }

  /**
   * @brief Adds a scalar value to each component of the vector.
   * @param aValue The scalar value to add.
   * @return Reference to this vector after addition.
   */
  MVector3& operator+=(PassType aValue)
    {
    translate(aValue, aValue, aValue);
    return *this;
    }

   /**
   * @brief Adds another vector to this vector.
   * @param aRight The vector to add.
   * @return Reference to this vector after addition.
   */
  MVector3& operator+=(const MVector3& aRight)
    {
    translate(aRight);
    return *this;
    }

  /**
   * @brief Subtracts another vector from this vector.
   * @param aRight The vector to subtract.
   * @return Reference to this vector after subtraction.
   */
  MVector3& operator-=(const MVector3& aRight)
    {
    translate(-aRight[0], -aRight[1], -aRight[2]);
    return *this;
    }

   /**
   * @brief Divides this vector by another vector element-wise.
   * @param aRight The vector to divide by.
   * @return Reference to this vector after division.
   */
  MVector3& operator/=(const MVector3& aRight)
    {
    theV[0] /= aRight[0];
    theV[1] /= aRight[1];
    theV[2] /= aRight[2];
    return *this;
    }

  /**
   * @brief Accesses the component at the specified index.
   * @param aIndex The index of the component (0, 1, or 2).
   * @return Reference to the component at the specified index.
   */
  Type& operator[] (unsigned int aIndex)
    { return theV[aIndex]; }

  /**
   * @brief Accesses the component at the specified index (const version).
   * @param aIndex The index of the component (0, 1, or 2).
   * @return Constant reference to the component at the specified index.
   */
  const Type& operator[] (unsigned int aIndex) const
    { return theV[aIndex]; }

 

   /**
   * @brief Returns a pointer to the vector data.
   * @return Pointer to the vector data.
   */
  const Type* getData() const
    { return theV; }

  // ----- Miscellaneous -----

  /**
   * @brief Sets the components of the vector.
   * @param a1 The first component.
   * @param a2 The second component.
   * @param a3 The third component.
   */
  void set(PassType a1,
           PassType a2,
           PassType a3);

   /**
   * @brief Translates (adds) the components of another vector to this vector.
   * @param aRight The vector to add.
   */
  void translate(const MVector3& aRight)
    { translate(aRight[0], aRight[1], aRight[2]); }

  /**
   * @brief Translates (adds) the specified values to the components of this vector.
   * @param a1 The value to add to the first component.
   * @param a2 The value to add to the second component.
   * @param a3 The value to add to the third component.
   */
  void translate(PassType a1,
                 PassType a2,
                 PassType a3);

  /**
   * @brief Computes the length (magnitude) of the vector.
   * @return The length of the vector.
   */
  Type length() const;

  /**
   * @brief Normalizes the vector (scales it to unit length).
   */
  void normalize();

   /**
   * @brief Calculates the dot product of this vector with another vector.
   *        If both vectors are normalized, this gives the cosine of the angle between them.
   * @param aOther The other vector.
   * @return The dot product of the two vectors.
   */
  Type dot(const MVector3& aOther) const;

  /**
   * @brief Calculates the cross product of two vectors.
   * @param aLeft The first vector.
   * @param aRight The second vector.
   * @return A new vector that is the cross product of aLeft and aRight.
   */
  MVector3 cross(const MVector3& aLeft,
                 const MVector3& aRight) const;

private:

  // ----- Fields -----

  /**
   * @brief The vector components.
   */
  Type theV[3];

  };


template <typename Type, typename PassType>
MVector3<Type, PassType>::MVector3()
  {
  memset(theV, 0, sizeof(Type) * 3);
  }

template <typename Type, typename PassType>
MVector3<Type, PassType>::~MVector3()
  {
  }

template <typename Type, typename PassType>
void MVector3<Type, PassType>::set(
    PassType a1,
    PassType a2,
    PassType a3)
  {
  theV[0] = a1;
  theV[1] = a2;
  theV[2] = a3;
  }

template <typename Type, typename PassType>
void MVector3<Type, PassType>::translate(
    PassType a1,
    PassType a2,
    PassType a3)
  {
  theV[0] += a1;
  theV[1] += a2;
  theV[2] += a3;
  }

template <typename Type, typename PassType>
Type MVector3<Type, PassType>::length() const
  {
  return (Type)sqrt(theV[0] * theV[0] +
                    theV[1] * theV[1] +
                    theV[2] * theV[2]);
  }

template <typename Type, typename PassType>
void MVector3<Type, PassType>::normalize()
  {
  Type len = length();
  theV[0] /= len;
  theV[1] /= len;
  theV[2] /= len;
  }

template <typename Type, typename PassType>
Type MVector3<Type, PassType>::dot(
    const MVector3& aOther) const
  {
  return (Type)(theV[0] * aOther.theV[0] +
                theV[1] * aOther.theV[1] +
                theV[2] * aOther.theV[2]);
  }

template <typename Type, typename PassType>
MVector3<Type, PassType> MVector3<Type, PassType>::cross(
    const MVector3& aLeft,
    const MVector3& aRight) const
  {
  MVector3 f;
  f[0] = aLeft[1] * aRight[2] - aLeft[2] * aRight[1];
  f[1] = aLeft[2] * aRight[0] - aLeft[0] * aRight[2];
  f[2] = aLeft[0] * aRight[1] - aLeft[1] * aRight[0];
  return f;
  }

typedef MVector3<float> MVector3f;
typedef MVector3<double> MVector3d;
typedef MVector3<int> MVector3i;
typedef MVector3<unsigned int> MVector3ui;



#endif