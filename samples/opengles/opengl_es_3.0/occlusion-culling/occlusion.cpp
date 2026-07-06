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

#include "XTime.h"
#include "common.hpp"
#include "scene.hpp"
#include "common.hpp"
#include <stdlib.h>

#define GLES_VERSION 3
#include "FontEngine.h"

#include "XTime.h"


#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

using namespace std;
using namespace SDKAMD;



#define GL_CHECK(x) \
        x; \
{ \
        GLenum glError = glGetError(); \
        if(glError != GL_NO_ERROR) { \
                printf("glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
                exit(1); \
        } \
}


static void render_text(FontEngine &text, const char *method, float current_time)
{
    // Enable alpha blending.
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    char method_string[128];
    sprintf(method_string, "Method: %s (%4.1f / 10.0 s)", method, current_time);

    text.clear();
    text.addString(300, WINDOW_H - 20, method_string, 255, 255, 255, 255);

    text.addString(20, WINDOW_H - 40,  "             Legend:", 255, 255, 255, 255);
    text.addString(20, WINDOW_H - 60,  "Green tinted sphere: LOD 0", 255, 255, 0, 255);
    text.addString(20, WINDOW_H - 80,  " Blue tinted sphere: LOD 1 - LOD 3", 255, 255, 0, 255);
    text.addString(20, WINDOW_H - 100, "        Dark sphere: Occluded spheres", 255, 255, 0, 255);

    text.draw();
    GL_CHECK(glDisable(GL_BLEND));
}

static void run_application(XPodium *platform)
{
    Scene *scene = new Scene;
    scene->set_show_redundant(true);
    scene->set_culling_method(Scene::CullHiZ);

    FontEngine *text = new FontEngine("Supportingfiles/", WINDOW_W, WINDOW_H);

    using namespace SDKAMD;
    Timer  timer;
    timer.reset();

    unsigned phase = 0;
    float culling_timer = 0.0f;

    while (platform->checkWindow() == XPodium::WINDOW_IDLE)
    {
        float delta_time = timer.getInterval();
                float total_time = timer.getTime();


        // Render scene.
        scene->move_camera(delta_time * 0.1f, 0.0f);
        scene->update(delta_time, WINDOW_W, WINDOW_H);
        scene->render(WINDOW_W, WINDOW_H);

        static const char *methods[] = {
            "Hierarchical-Z occlusion culling with level-of-detail",
            "Hierarchical-Z occlusion culling without level-of-detail",
            "No culling"
        };
        render_text(*text, methods[phase], culling_timer);

        	    /* Calculate FPS. */

        if(timer.isTimePassed(1.0f))
        {
            printf("FPS: %.2f\n", (1.0f / delta_time));
        }


        // Don't need depth nor stencil buffers anymore. Just discard them so they are not written out to memory on Mali.
        static const GLenum attachments[] = { GL_DEPTH, GL_STENCIL };
        GL_CHECK(glInvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments));

        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);

        // Change the culling method over time.
        culling_timer += delta_time;
        if (culling_timer > 10.0f)
        {
            culling_timer = 0.0f;
            phase = (phase + 1) % 3;

            switch (phase)
            {
                case 0:
                    scene->set_culling_method(Scene::CullHiZ);
                    break;
                case 1:
                    scene->set_culling_method(Scene::CullHiZNoLOD);
                    break;
                case 2:
                    scene->set_culling_method(Scene::CullNone);
                    break;
            }
        }
    }

    delete text;
    delete scene;
}

int main()
{

    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES31);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);

    common_set_basedir("Supportingfiles");
    run_application(podium);


    CoreEGL::terminateEGL();
    /* Shut down windowing system. */
    podium->destroyWindow();

    /* Shut down the Platform object. */
    delete podium;
    /* [Release resources] */

    return EXIT_SUCCESS;
}

