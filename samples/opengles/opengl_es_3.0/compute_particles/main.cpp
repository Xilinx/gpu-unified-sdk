/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
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
