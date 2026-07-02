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

#include "XTime.h"

#if defined(_WIN32)

#include <cstdio>
#include <windows.h>

namespace SDKXilinx
{
    /**
     * @brief Constructor for Timer class.
     * 
     * Initializes the timer by setting up performance counter frequency for high-resolution
     * timing on Windows systems. Resets all timing variables and starts the timer.
     */
    Timer::Timer()
    {
        LARGE_INTEGER l;
        QueryPerformanceFrequency(&l);
        invFreq = 1.0f / l.QuadPart;
        lastInterval = 0.0f;
        frameCount = 0;
        lastFpsUpdate = 0.0f;
        reset();
        fps = 0.0f;
        lastTime = 0.0f;
    }

    /**
     * @brief Resets the timer to start measuring from current time.
     * 
     * Records the current high-resolution performance counter value as the new
     * reference point for all subsequent time measurements.
     */
    void Timer::reset()
    {
        LARGE_INTEGER l;
        QueryPerformanceCounter(&l);
        resetStamp = (((double)l.QuadPart) * invFreq);
    }

    /**
     * @brief Gets the elapsed time since the last reset.
     * 
     * Uses Windows high-resolution performance counter to calculate the time elapsed
     * since the timer was last reset.
     * 
     * @return Elapsed time in seconds as a float
     */
    float Timer::getTime()
    {
        LARGE_INTEGER l;
        QueryPerformanceCounter(&l);
        return (float)(((double)l.QuadPart) * invFreq - resetStamp);
    }

    /**
     * @brief Gets the time interval since the last call to getInterval().
     * 
     * Calculates the time difference between the current time and the time when
     * this method was last called, useful for frame timing calculations.
     * 
     * @return Time interval in seconds as a float
     */
    float Timer::getInterval()
    {
        float time = getTime();
        float interval = time - lastInterval;
        lastInterval = time;
        return interval;
    }

    /**
     * @brief Calculates and returns the current frames per second.
     * 
     * Updates the FPS calculation every second by counting frames and averaging
     * over the elapsed time. Call this method once per frame for accurate results.
     * 
     * @return Current FPS as a float
     */
    float Timer::getFPS()
    {
        float time = getTime();
        frameCount++;
        if (time-lastFpsUpdate > 1.0f)
        {
            fps = (float)frameCount / (time-lastFpsUpdate);
            lastFpsUpdate = time;
            frameCount = 0;
        }
        return fps;
    }
}
#else

#include <sys/time.h>

namespace SDKXilinx
{
    /**
     * @brief Constructor for Timer class (Unix/Linux version).
     * 
     * Initializes the timer by setting up timing variables and calling reset()
     * to establish the initial time reference point using gettimeofday().
     */
    Timer::Timer()
        : startTime()
        , currentTime()
        , lastIntervalTime(0.0f)
        , frameCount(0)
        , fpsTime(0.0f)
        , fps(0.0f)
    {    
        startTime.tv_sec = 0;
        startTime.tv_usec = 0;
        currentTime.tv_sec = 0;
        currentTime.tv_usec = 0;

        reset();
    }

    /**
     * @brief Resets the timer to start measuring from current time.
     * 
     * Records the current time using gettimeofday() as the new reference point
     * for all subsequent time measurements. Also resets FPS calculation variables.
     */
    void Timer::reset()
    {
        gettimeofday(&startTime, NULL);
        lastIntervalTime = 0.0;

        frameCount = 0;
        fpsTime = 0.0f;
    }

    /**
     * @brief Gets the elapsed time since the last reset.
     * 
     * Uses gettimeofday() to calculate the time elapsed since the timer was last reset.
     * Provides microsecond precision by combining seconds and microseconds.
     * 
     * @return Elapsed time in seconds as a float
     */
    float Timer::getTime()
    {
        gettimeofday(&currentTime, NULL);
        float seconds = (currentTime.tv_sec - startTime.tv_sec);
        float milliseconds = (float(currentTime.tv_usec - startTime.tv_usec)) / 1000000.0f;
        return seconds + milliseconds;
    }

    /**
     * @brief Gets the time interval since the last call to getInterval().
     * 
     * Calculates the time difference between the current time and the time when
     * this method was last called, useful for frame timing calculations.
     * 
     * @return Time interval in seconds as a float
     */
    float Timer::getInterval()
    {
        float time = getTime();
        float interval = time - lastIntervalTime;
        lastIntervalTime = time;
        return interval;
    }

    /**
     * @brief Calculates and returns the current frames per second.
     * 
     * Updates the FPS calculation every second by counting frames and averaging
     * over the elapsed time. Call this method once per frame for accurate results.
     * 
     * @return Current FPS as a float
     */
    float Timer::getFPS()
    {
        if (getTime() - fpsTime > 1.0f)
        {
            fps = static_cast<float>(frameCount) / (getTime() - fpsTime);
            frameCount = 0;
            fpsTime = getTime();
        }
        ++frameCount;
        return fps;
    }
#endif

    bool Timer::isTimePassed(float seconds)
    {
        float time = getTime();
        if (time - lastTime > seconds)
        {
            lastTime = time;
            return true;
        }
        return false;
    }

}
