/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/**
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 *
*******************************************************************************/
/**
 *
 * @file XTime.h
 * @brief This file implements timer related API for SDK
 *
*******************************************************************************/
#ifndef TIMER_H
#define TIMER_H

#include <cstdio>

#if defined(_WIN32)
#else
#include <sys/time.h>
#endif

namespace SDKXilinx
{

    /**
     * @class Timer
     * @brief A class for measuring elapsed time and calculating frames per second (FPS).
     * 
     * This class provides methods to measure the elapsed time, calculate the time interval
     * between calls, and determine the frames per second. It supports basic time management
     * functionalities to facilitate performance monitoring and timing in applications.
     */
    class Timer
    {
    private:
        int frameCount; ///< Counter for the number of frames.
        float fps; ///< Frames per second.
        float lastTime; ///< Time of the last measurement.
        timeval startTime; ///< Start time for the timer.
        timeval currentTime; ///< Current time for measurement.
        float lastIntervalTime; ///< Time of the last interval measurement.
        float fpsTime; ///< Time used for calculating FPS.

    public:
        /**
         * @brief Default constructor.
         * 
         * Initializes the Timer object and sets up initial time values.
         */
        Timer();

        /**
         * @brief Resets the timer.
         * 
         * Resets the internal counters and time measurements to start a new timing session.
         */
        void reset();

        /**
         * @brief Gets the current time.
         * 
         * @return The elapsed time in seconds since the last reset as a float.
         */
        float getTime();

        /**
         * @brief Gets the time interval since the last call to getInterval().
         * 
         * @return The time interval in seconds as a float.
         */
        float getInterval();

        /**
         * @brief Gets the frames per second (FPS).
         * 
         * @return The calculated FPS as a float.
         */
        float getFPS();

        /**
         * @brief Checks if a specified amount of time has passed.
         * 
         * This method determines if the specified number of seconds has elapsed since
         * the last call to this method or since the timer was last reset.
         * 
         * @param seconds The number of seconds to check for. Default is 1.0 seconds.
         * @return True if the specified time has passed, false otherwise.
         */
        bool isTimePassed(float seconds = 1.0f);
    };

}

#endif /* TIMER_H */
