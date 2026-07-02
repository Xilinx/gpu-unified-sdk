/* Copyright (c) 2014-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Modifications Copyright (C) 2024-2026 Advanced Micro Devices, Inc.  All rights reserved.
 */

#include <stdlib.h>

#include "app.h"
#include "common/common.h"

#include "XTime.h"


#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

using namespace SDKXilinx;

/**
 * @brief Main application entry point for compute shader particle system demonstration
 * 
 * @return Exit status code (EXIT_SUCCESS for success, EXIT_FAILURE for failure)
 * 
 * This advanced OpenGL ES 3.1 application demonstrates compute shader-based particle systems:
 * - Utilizes compute shaders for GPU-accelerated particle physics simulation
 * - Implements massive particle systems with hundreds of thousands of particles
 * - Showcases OpenGL ES 3.1 compute shader capabilities and limitations
 * - Demonstrates proper synchronization between compute and rendering passes
 * 
 * Key OpenGL ES 3.1 features demonstrated:
 * - Compute shaders for parallel particle updates
 * - Shader Storage Buffer Objects (SSBOs) for particle data
 * - Memory barriers and synchronization primitives
 * - Transform feedback for efficient GPU-to-GPU data flow
 * - Indirect rendering for dynamic geometry generation
 * 
 * This sample represents cutting-edge GPU programming techniques suitable for
 * high-performance simulations, scientific computing, and advanced game effects
 * that require massive parallel processing capabilities.
 */
int main()
{


    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES31);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);


    Timer timer;
    timer.reset();

    if (!load_app())
    {
        return EXIT_FAILURE;
    }


    init_app(WINDOW_W, WINDOW_H);


    /* ChronoEngine variable to calculate FPS. */

       bool end = false;
       static float time = 0.0;

    while (!end)
    {

        float delta_time = timer.getInterval();
        float total_time = timer.getTime();
        update_app(delta_time, total_time);

	/* Calculate FPS. */

        if(timer.isTimePassed(1.0f))
        {
            float FPS = timer.getFPS();
            printf("FPS: %.2f\n", (1.0f / delta_time));
           LOGI("FPS:\t%.1f", FPS);
        }

        render_app(delta_time, total_time);

	eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }


    free_app();

    CoreEGL::terminateEGL();
    /* Shut down windowing system. */
    podium->destroyWindow();

    /* Shut down the Platform object. */
    /* [Release resources] */

    return EXIT_SUCCESS;
}
