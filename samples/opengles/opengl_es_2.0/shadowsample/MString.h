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
 * @file MString.h
 *
 * @brief This class represents a basic string with customizable character types.
 *
*******************************************************************************/

#ifndef M_SHADOWMAP_STRING_HPP
#define M_SHADOWMAP_STRING_HPP

#include "ShadowMap.h"
#include "MArray.h"

/**
 * @brief A template class representing a basic string with customizable character types.
 * 
 * This class provides basic string operations and supports different character types. It
 * manages a sequence of characters and provides methods for string manipulation and querying.
 * 
 * @tparam Type The type of characters stored in the string (e.g., char, wchar_t).
 * @tparam TypeOffset The type used for indexing and length (default: unsigned int).
 */
template <typename Type, typename TypeOffset = unsigned int>
class MStringBase
{
public:

    // ----- Types -----

    /**
     * @brief Alias for the character type used in the string.
     */
    typedef Type CharType;

    /**
     * @brief Alias for the type used for indexing and length.
     */
    typedef TypeOffset CharOffset;

    // ----- Statics and constants -----

    /**
     * @brief Constant value indicating that a character was not found.
     */
    static const CharOffset theNotFound;

    // ----- Constructors and destructors -----

    /**
     * @brief Default constructor for MStringBase.
     * 
     * Initializes an empty string.
     */
    MStringBase()
    {}

    /**
     * @brief Copy constructor for MStringBase.
     * 
     * Creates a new MStringBase instance as a copy of the given string.
     * 
     * @param aOther The string to copy from.
     */
    MStringBase(const MStringBase<Type, TypeOffset>& aOther);

    /**
     * @brief Constructor that initializes the string from an ASCII C-string.
     * 
     * @param aAscii The ASCII C-string to initialize the string from.
     */
    MStringBase(const char* aAscii);

    /**
     * @brief Constructor that initializes the string from a sequence of characters.
     * 
     * @param aCharacters Pointer to the sequence of characters.
     * @param aCountCharacters The number of characters in the sequence.
     */
    MStringBase(const Type* aCharacters, TypeOffset aCountCharacters);

    /**
     * @brief Destructor for MStringBase.
     * 
     * Cleans up any resources used by the string.
     */
    ~MStringBase()
    {}

    // ----- Operators -----

    /**
     * @brief Assignment operator for MStringBase.
     * 
     * Copies the contents of the given string to this instance.
     * 
     * @param aOther The string to assign from.
     * @return A reference to this string.
     */
    MStringBase& operator=(const MStringBase& aOther);

    /**
     * @brief Append operator for MStringBase.
     * 
     * Appends the contents of the given string to this instance.
     * 
     * @param aOther The string to append.
     * @return A reference to this string.
     */
    MStringBase& operator+=(const MStringBase& aOther);

    /**
     * @brief Equality operator for MStringBase.
     * 
     * Checks if this string is equal to the given string.
     * 
     * @param aOther The string to compare to.
     * @return True if the strings are equal, false otherwise.
     */
    bool operator==(const MStringBase& aOther) const
    { return (theInternalString == aOther.theInternalString); }

    /**
     * @brief Subscript operator for accessing characters by index.
     * 
     * @param aIndex The index of the character to access.
     * @return A reference to the character at the given index.
     */
    Type& operator[] (TypeOffset aIndex)
    { return theInternalString[aIndex]; }

    /**
     * @brief Subscript operator for accessing characters by index (const).
     * 
     * @param aIndex The index of the character to access.
     * @return A const reference to the character at the given index.
     */
    const Type& operator[] (TypeOffset aIndex) const
    { return theInternalString[aIndex]; }

    // ----- Accessors and mutators -----

    // ----- Miscellaneous -----

    /**
     * @brief Retrieves the length of the string.
     * 
     * @return The length of the string, excluding the null terminator.
     */
    TypeOffset getLength() const;

    /**
     * @brief Retrieves a pointer to the internal character data.
     * 
     * @return A pointer to the internal character data.
     */
    const Type* getData() const
    { return theInternalString.getData(); }

    /**
     * @brief Clears the contents of the string.
     * 
     * Removes all characters from the string.
     */
    void clear()
    { theInternalString.clear(); }

    /**
     * @brief Appends a single character to the end of the string.
     * 
     * @param aChar The character to append.
     */
    void append(Type aChar);

    /**
     * @brief Appends another string to the end of this string.
     * 
     * @param aChar The string to append.
     */
    void append(const MStringBase<Type, TypeOffset>& aChar);

    /**
     * @brief Sets the contents of the string from an ASCII C-string.
     * 
     * @param aAscii The ASCII C-string to set the string to.
     */
    void set(const char* aAscii);

    /**
     * @brief Finds the first occurrence of a character in the string.
     * 
     * @param aChar The character to find.
     * @param aStartOffset The offset to start the search from (default: 0).
     * @return The index of the first occurrence of the character, or theNotFound if not found.
     */
    TypeOffset find(CharType aChar, TypeOffset aStartOffset = 0) const;

    /**
     * @brief Extracts a substring from this string.
     * 
     * @param aStartOffset The start index of the substring.
     * @param aEndOffset The end index of the substring.
     * @param aOutStr The string to store the extracted substring.
     */
    void sub(TypeOffset aStartOffset, TypeOffset aEndOffset, MStringBase<Type, TypeOffset>& aOutStr) const;

    /**
     * @brief Converts the string to an integer.
     * 
     * @return The integer value of the string, or 0 if the string is empty or cannot be converted.
     */
    int getAsInt() const;

private:

    // ----- Fields -----

    /**
     * @brief Internal storage for the characters in the string.
     */
    MArray<Type> theInternalString;
};

// Initialize theNotFound constant
template<typename Type, typename TypeOffset>
const TypeOffset MStringBase<Type, TypeOffset>::theNotFound = MArray<Type>::NOT_FOUND;

// Constructor implementations
template <typename Type, typename TypeOffset>
MStringBase<Type, TypeOffset>::MStringBase(const char* aAscii)
{
    set(aAscii);
}

template <typename Type, typename TypeOffset>
MStringBase<Type, TypeOffset>::MStringBase(const MStringBase<Type, TypeOffset>& aOther)
    : theInternalString()
{
    theInternalString = aOther.theInternalString;
}

template <typename Type, typename TypeOffset>
MStringBase<Type, TypeOffset>& MStringBase<Type, TypeOffset>::operator=(
    const MStringBase<Type, TypeOffset>& aOther)
{
    theInternalString = aOther.theInternalString;
    return *this;
}

template <typename Type, typename TypeOffset>
MStringBase<Type, TypeOffset>& MStringBase<Type, TypeOffset>::operator+=(
    const MStringBase<Type, TypeOffset>& aOther)
{
    append(aOther);
    return *this;
}

template <typename Type, typename TypeOffset>
TypeOffset MStringBase<Type, TypeOffset>::getLength() const
{
    TypeOffset len = theInternalString.getCount();
    if (len)
        len--; // decrease because of ZERO at the end of string
    return len;
}

template <typename Type, typename TypeOffset>
void MStringBase<Type, TypeOffset>::set(const char* aAscii)
{
    if (sizeof(char) == sizeof(Type))
    {
        theInternalString.set(aAscii, (TypeOffset)strlen(aAscii));
        theInternalString.append(0);
    }
    else
    {
        TypeOffset len = (TypeOffset)strlen(aAscii);
        theInternalString.setCount(len + 1);
        for (TypeOffset index = 0; index < len; index++)
            theInternalString[index] = (Type)aAscii[index];
        theInternalString[len] = 0;
    }
}

template <typename Type, typename TypeOffset>
TypeOffset MStringBase<Type, TypeOffset>::find(
    CharType aChar,
    TypeOffset aStartOffset) const
{
    return theInternalString.find(aChar, aStartOffset);
}

template <typename Type, typename TypeOffset>
void MStringBase<Type, TypeOffset>::sub(
    TypeOffset aStartOffset,
    TypeOffset aEndOffset,
    MStringBase<Type, TypeOffset>& aOutStr) const
{
    aOutStr.clear();
    // Prevent not going to out of memory
    if (aStartOffset >= getLength() || aEndOffset >= getLength())
        return;
    // Copy characters
    for (TypeOffset offset = aStartOffset; offset <= aEndOffset; offset++)
        aOutStr.append((*this)[offset]);
}

template <typename Type, typename TypeOffset>
void MStringBase<Type, TypeOffset>::append(
    Type aChar)
{
    TypeOffset count = theInternalString.getCount();
    if (count == 0)
        // Add first character to the array
        theInternalString.append(aChar);
    else
        // Replace last element which is a value of ZERO
        theInternalString[theInternalString.getCount() - 1] = aChar;
    // Append ZERO to the end of string
    theInternalString.append(0);
}

template <typename Type, typename TypeOffset>
void MStringBase<Type, TypeOffset>::append(
    const MStringBase<Type, TypeOffset>& aOther)
{
    TypeOffset count = theInternalString.getCount();
    if (count != 0)
        // Remove last ZERO element from the array
        theInternalString.setCount(count - 1);
    // Simply add other string because it already has its own ZERO at the end
    theInternalString.append(aOther.theInternalString);
}

template <typename Type, typename TypeOffset>
int MStringBase<Type, TypeOffset>::getAsInt() const
{
    if (getLength())
        return atoi(theInternalString.getData());
    else
        return 0;
}

// Type aliases for common usage
typedef MStringBase<char, unsigned int> MString; ///< String type for ASCII characters.
typedef MStringBase<char, unsigned int> MPath;   ///< Path type for ASCII characters.

#endif // M_SHADOWMAP_STRING_HPP
