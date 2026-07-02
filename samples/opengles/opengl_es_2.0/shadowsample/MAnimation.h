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
 * @file MAnimation.h
 *
 * @brief This file implements animation effects.
 *
*******************************************************************************/
#ifndef M_SHADOWMAP_ANIMATION_HPP
#define M_SHADOWMAP_ANIMATION_HPP

#include "ShadowMap.h"
#include "MArray.h"
#include "MTime.h"

/**
 * @brief A template class for managing animations with keyframes.
 * 
 * This class provides functionality for creating and managing animations
 * that can be updated over time. Keyframes can be added and the animation
 * can be played in different modes.
 * 
 * @tparam Type The type of the value to be animated.
 * @tparam PassType The type used for passing values. Defaults to Type.
 * @tparam ReturnType The type used for returning values. Defaults to PassType.
 */
template <typename Type, typename PassType = Type, typename ReturnType = PassType>
class MAnimation {
public:
    /**
     * @brief Enum for specifying the play mode of the animation.
     */
    enum MDRPlayMode {
        PLAY_MODE_ONCE,  ///< Play the animation once.
        PLAY_MODE_REPEAT ///< Play the animation repeatedly.
    };

    /**
     * @brief Default constructor for MAnimation.
     * Initializes the animation with a default value and play mode.
     */
    MAnimation() : theValue((Type)0), theMode(PLAY_MODE_ONCE) {}

    /**
     * @brief Destructor for MAnimation.
     */
    ~MAnimation() {}

    /**
     * @brief Gets the current value of the animation.
     * 
     * @return The current value of the animation.
     */
    ReturnType getValue() const { return theValue; }

    /**
     * @brief Updates the animation based on the current time.
     * 
     * @param aTime The current time to update the animation.
     */
    void update(const MTime& aTime);

    /**
     * @brief Sets a keyframe for the animation.
     * 
     * @param aValue The value to set at the given time.
     * @param aTime The time at which the keyframe should occur.
     */
    void setKey(PassType aValue, const MTime& aTime);

    /**
     * @brief Sets the play mode of the animation.
     * 
     * @param aMode The play mode to set for the animation.
     */
    void setMode(MDRPlayMode aMode) { theMode = aMode; }

    /**
     * @brief Removes all keyframes from the animation.
     */
    void removeAllKeys() { theKeys.clear(); }

private:
    /**
     * @brief A class representing a single keyframe in the animation.
     */
    class MDRKey {
    public:
        /**
         * @brief Constructs a keyframe with a given value and time.
         * 
         * @param aValue The value of the keyframe.
         * @param aTime The time at which the keyframe occurs.
         */
        MDRKey(PassType aValue, const MTime& aTime) : theValue(aValue), theTime(aTime) {}

        /**
         * @brief Assignment operator for MDRKey.
         * 
         * @param aOther The keyframe to copy from.
         * @return A reference to this keyframe.
         */
        MDRKey& operator = (const MDRKey& aOther) {
            theValue = aOther.theValue;
            theTime = aOther.theTime;
            return *this;
        }

        friend class MAnimation<Type, PassType, ReturnType>; ///< Allow MAnimation to access private members.
    private:
        PassType theValue; ///< The value of the keyframe.
        MTime theTime;     ///< The time at which the keyframe occurs.
    };

    typedef MArray<MDRKey> MDRKeys; ///< Array of keyframes.
    Type theValue; ///< The current value of the animation.
    MDRPlayMode theMode; ///< The play mode of the animation.
    MDRKeys theKeys; ///< The list of keyframes.

    /**
     * @brief Performs linear interpolation between two values.
     * 
     * @param aCoefficient The interpolation coefficient (0.0 to 1.0).
     * @param aLeft The starting value for interpolation.
     * @param aRight The ending value for interpolation.
     * @param aReturnValue The interpolated value.
     */
    void lerp(double aCoefficient, PassType aLeft, PassType aRight, Type& aReturnValue) const;
};

template <typename Type, typename PassType, typename ReturnType>
void MAnimation<Type, PassType, ReturnType>::setKey(PassType aValue, const MTime& aTime) {
    theKeys.append(MDRKey(aValue, aTime));
}

template <typename Type, typename PassType, typename ReturnType>
void MAnimation<Type, PassType, ReturnType>::update(const MTime& aTime) {
    unsigned int count = theKeys.getCount();
    if (count <= 1) {
        if (count == 1) theValue = theKeys[0].theValue;
        return;
    }

    if (theMode == PLAY_MODE_REPEAT) {
        MTime range = theKeys[count - 1].theTime - theKeys[0].theTime;
        double elapsed = aTime.getSeconds() - range.getSeconds() * floor((aTime.getSeconds() - theKeys[0].theTime.getSeconds()) / range.getSeconds());
        MTime currentTime(elapsed);

        for (unsigned int i = 1; i < count; i++) {
            if (currentTime >= theKeys[i - 1].theTime && currentTime <= theKeys[i].theTime) {
                double coefficient = (currentTime.getSeconds() - theKeys[i - 1].theTime.getSeconds()) / (theKeys[i].theTime.getSeconds() - theKeys[i - 1].theTime.getSeconds());
                lerp(coefficient, theKeys[i - 1].theValue, theKeys[i].theValue, theValue);
                break;
            }
        }
    }
}

typedef MAnimation<float> MAnimation1f; ///< Animation with float values.
typedef MAnimation<int> MAnimation1i; ///< Animation with int values.

#endif /* M_SHADOWMAP_ANIMATION_HPP */