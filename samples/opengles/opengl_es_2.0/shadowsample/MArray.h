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
 * @file MArray.h
 *
 * @brief This class represents an array template type. The most important thing about arrays in general is that they tend to keep elements in contiguous memory.
 * That being said they are cache friendly.
 *
*******************************************************************************/
#ifndef M_SHADOWMAP_ARRAY_HPP
#define M_SHADOWMAP_ARRAY_HPP

//------------------------------------------
// INCLUDES

#include "ShadowMap.h"

//------------------------------------------
// BEGIN OF CLASS DECLARATION

/**
 * @brief A template class for a dynamic array with basic functionalities.
 * 
 * This class provides a dynamic array implementation with support for resizing,
 * appending, and accessing elements. It uses memory alignment for efficiency and
 * provides methods to manipulate and query the array.
 * 
 * @tparam Type The type of elements stored in the array.
 * @tparam TypeOffset The type used for indexing and sizing the array. Defaults to unsigned int.
 */
template <typename Type, typename TypeOffset = unsigned int>
class MArray
{
public:

    // ----- Types -----

    /**
     * @brief Constants related to the array.
     */
    enum
    {
        ARRAY_MEM_ALIGN = 32, ///< Memory alignment for array elements.
        NOT_FOUND = 0xFFFFFFFF ///< Constant indicating that an element was not found.
    };

    /**
     * @brief Type alias for elements in the array.
     */
    typedef Type ElementType;

    /**
     * @brief Type alias for offset/indexing the array.
     */
    typedef TypeOffset OffsetType;

    /**
     * @brief Iterator type for non-const access to array elements.
     */
    typedef Type* MDRIterator;

    /**
     * @brief Iterator type for const access to array elements.
     */
    typedef const Type* MDRConstIterator;

    // ----- Constructors and destructors -----

    /**
     * @brief Constructs an empty array with a specified initial size.
     * 
     * @param aSize The initial size of the array. Defaults to 16.
     */
    MArray(TypeOffset aSize = 16);

    /**
     * @brief Destructs the array, freeing allocated memory.
     */
    ~MArray();

    // ----- Operators -----

    /**
     * @brief Assignment operator for copying the contents of another array.
     * 
     * @param aOther The array to copy from.
     * @return A reference to this array.
     */
    MArray& operator=(const MArray& aOther);

    /**
     * @brief Equality operator for comparing two arrays.
     * 
     * @param aOther The array to compare with.
     * @return True if the arrays are equal, false otherwise.
     */
    bool operator==(const MArray& aOther) const;

    /**
     * @brief Accesses an element at a specific index.
     * 
     * @param aIndex The index of the element.
     * @return A reference to the element at the given index.
     */
    Type& operator[] (TypeOffset aIndex)
    { return theData[aIndex]; }

    /**
     * @brief Accesses an element at a specific index (const version).
     * 
     * @param aIndex The index of the element.
     * @return A const reference to the element at the given index.
     */
    const Type& operator[] (TypeOffset aIndex) const
    { return theData[aIndex]; }

    /**
     * @brief Gets an iterator to the beginning of the array.
     * 
     * @return An iterator to the first element of the array.
     */
    MDRConstIterator getBegin() const
    { return &theData[0]; }

    /**
     * @brief Gets an iterator to the end of the array.
     * 
     * @return An iterator to the position just past the last element.
     */
    MDRConstIterator getEnd() const
    { return getBegin() + theCount; }

    /**
     * @brief Gets a non-const iterator to the beginning of the array.
     * 
     * @return A non-const iterator to the first element of the array.
     */
    MDRIterator getBegin()
    { return &theData[0]; }

    /**
     * @brief Gets a non-const iterator to the end of the array.
     * 
     * @return A non-const iterator to the position just past the last element.
     */
    MDRIterator getEnd()
    { return getBegin() + theCount; }

    // ----- Accessors and mutators -----

    /**
     * @brief Gets the number of elements currently in the array.
     * 
     * @return The count of active elements in the array.
     */
    TypeOffset getCount() const
    { return theCount; }

    /**
     * @brief Gets a pointer to the data buffer of the array.
     * 
     * @return A pointer to the data buffer.
     */
    const Type* getData() const
    { return theData; }

    /**
     * @brief Sets the number of active elements in the array.
     * 
     * @param aCount The new number of active elements.
     */
    void setCount(TypeOffset aCount);

    // ----- Miscellaneous -----

    /**
     * @brief Copies the contents from another array.
     * 
     * @param aOther The array to copy from.
     */
    void set(const MArray& aOther);

    /**
     * @brief Appends an element to the end of the array.
     * 
     * @param aElement The element to append.
     */
    void append(const Type& aElement);

    /**
     * @brief Appends the contents of another array to this array.
     * 
     * @param aOther The array whose contents to append.
     */
    void append(const MArray& aOther);

    /**
     * @brief Clears the array, setting the count to zero.
     */
    void clear();

    /**
     * @brief Fills the array with a specified value.
     * 
     * @param aValue The value to fill the array with.
     */
    void fill(Type aValue);

    /**
     * @brief Sets the array data from a pointer.
     * 
     * @param aData A pointer to the data to set.
     * @param aCount The number of elements to set.
     */
    void set(const Type* aData, TypeOffset aCount);

    /**
     * @brief Finds the index of the first occurrence of an element.
     * 
     * @param aElement The element to find.
     * @param aStartOffset The starting index for the search. Defaults to 0.
     * @return The index of the element, or NOT_FOUND if not found.
     */
    TypeOffset find(Type aElement, TypeOffset aStartOffset = 0) const;

private:

    // ----- Fields -----

    Type *theData; ///< Pointer to the array data buffer.
    TypeOffset theSize; ///< The allocated size of the buffer.
    TypeOffset theCount; ///< The number of active elements in the array.

    // ----- Forbidden Copy Constructor -----

    /**
     * @brief Private copy constructor to prevent copying.
     * 
     * @param aOther The array to copy from.
     */
    MArray(const MArray& aOther);

    // ----- Miscellaneous -----

    /**
     * @brief Reallocates the array buffer if needed to accommodate more elements.
     * 
     * @param aExpectedCountOfElements The expected number of elements after reallocation.
     */
    void reallocateIfNeeded(TypeOffset aExpectedCountOfElements);
};

// ----- Template Method Implementations -----

template <typename Type, typename TypeOffset>
MArray<Type, TypeOffset>::MArray(TypeOffset aSize)
    : theData(NULL), theSize(aSize), theCount(0)
{
    reallocateIfNeeded(theSize);
}

template <typename Type, typename TypeOffset>
MArray<Type, TypeOffset>::~MArray()
{
    if (theData != NULL)
    {
        free(theData);
        theData = NULL;
    }
}

template <typename Type, typename TypeOffset>
MArray<Type, TypeOffset>& MArray<Type, TypeOffset>::operator=(const MArray& aOther)
{
    set(aOther);
    return *this;
}

template <typename Type, typename TypeOffset>
bool MArray<Type, TypeOffset>::operator==(const MArray& aOther) const
{
    // Compare length
    if (theCount != aOther.getCount())
    {
        return false;
    }
    // Compare each element
    for (TypeOffset index = 0; index < theCount; index++)
    {
        if (theData[index] != aOther.theData[index])
        {
            return false;
        }
    }
    // It seems that both arrays are the same
    return true;
}

template <typename Type, typename TypeOffset>
void MArray<Type, TypeOffset>::set(const MArray& aOther)
{
    setCount(aOther.getCount());
    if (aOther.getCount() > 0)
    {
        memcpy(theData, aOther.theData, sizeof(Type) * aOther.getCount());
    }
}

template <typename Type, typename TypeOffset>
void MArray<Type, TypeOffset>::append(const Type& aElement)
{
    setCount(theCount + 1);
    theData[theCount - 1] = aElement;
}

template <typename Type, typename TypeOffset>
void MArray<Type, TypeOffset>::append(const MArray& aOther)
{
    TypeOffset prevCount = theCount;
    setCount(theCount + aOther.getCount());
    memcpy(theData + prevCount, aOther.theData, aOther.getCount() * sizeof(Type));
}

template <typename Type, typename TypeOffset>
void MArray<Type, TypeOffset>::clear()
{
    setCount(0);
}

template <typename Type, typename TypeOffset>
void MArray<Type, TypeOffset>::fill(Type aValue)
{
    memset(theData, aValue, theSize * sizeof(Type));
}

template <typename Type, typename TypeOffset>
void MArray<Type, TypeOffset>::set(const Type* aData, TypeOffset aCount)
{
    setCount(aCount);
    memcpy(theData, aData, sizeof(Type) * aCount);
}

template <typename Type, typename TypeOffset>
TypeOffset MArray<Type, TypeOffset>::find(Type aElement, TypeOffset aStartOffset /* = 0 */) const
{
    // Prevent going out of bound of array
    if (aStartOffset >= theCount)
    {
        return NOT_FOUND;
    }
    // Compare each element
    for (TypeOffset offset = aStartOffset; offset < theCount; offset++)
    {
        if (theData[offset] == aElement)
        {
            return offset;
        }
    }
    return NOT_FOUND;
}

template <typename Type, typename TypeOffset>
void MArray<Type, TypeOffset>::setCount(TypeOffset aCount)
{
    reallocateIfNeeded(aCount);
    theCount = aCount;
}

template <typename Type, typename TypeOffset>
void MArray<Type, TypeOffset>::reallocateIfNeeded(TypeOffset aExpectedCountOfElements)
{
    // Align memory for faster allocation
    TypeOffset newSize = ((aExpectedCountOfElements / ARRAY_MEM_ALIGN) + 1) * ARRAY_MEM_ALIGN;
    // Allocate the memory unconditionally
    theData = (Type*)realloc(theData, newSize * sizeof(Type));
    theSize = newSize;
}

#endif /* M_SHADOWMAP_ARRAY_HPP */