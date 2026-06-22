/*******************************************************************************
 *
 * Copyright (C) 2023 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 *******************************************************************************/
/******************************************************************************/
/**
 *
 * @file WaverenderSample.cpp
 *
 * This file implements test application for OpenGL rendering using SDK
 *
 * @note        None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        -----------------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * </pre>
 *
 *******************************************************************************/
/******************************* Source Files ********************************/

#include "WaverenderSample.h"
#include "FontEngine.h"
#include "XEGLIntf.h"
#include "XGLSLCompile.h"
#include "XMatrixAPI.h"
#include "XPodium.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

using std::string;

using namespace SDKXilinx;

/* A text object to draw text on the screen. */
FontEngine *text;


#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

string fontResourceDirectory = "FontShaders/";
string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "WaverenderSample.vert";
string fragmentShaderFilename = "WaverenderSample.frag";

GLuint programID;
GLint iLocPosition = -1;
GLint itimeLocation = -1;
GLint iLocMVP = -1;

/**
 * @brief Graphics rendering function for animated wave form demonstration
 * 
 * @param time Current time value for wave animation
 * 
 * This function renders an animated waveform using vertex shader displacement:
 * - Passes time uniform to vertex shader for wave calculations
 * - Applies 3D transformations (rotation and translation) to the wave mesh
 * - Uses procedural vertex displacement to create realistic wave motion
 * - Renders continuously updating wave geometry with smooth animation
 * 
 * The wave effect is achieved through vertex shader manipulation of a grid mesh,
 * where each vertex's position is modified based on sine/cosine functions and time,
 * creating a realistic fluid wave animation entirely on the GPU.
 */
void GraphicsUpdate(float time) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
    glUniform1f(itimeLocation, time);
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, Vertices);
    glEnableVertexAttribArray(iLocPosition);

    static float angleX = 0, angleY = 0, angleZ = 0;
    XMatrixAPI modelView = XMatrixAPI::createRotationX(angleX);
    XMatrixAPI translate1 = XMatrixAPI::createTranslation(0.0, 0.0, -1.5);

    modelView = translate1 * modelView;

    XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, 16 / 9, 0.01f, 100.0f);
    XMatrixAPI modelViewPerspective = perspective * modelView;

    glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());

    glDrawArrays(GL_TRIANGLES, 0, 36);
    text->draw();
}

/**
 * @brief Initializes graphics resources for wave form animation
 * 
 * @param width Window width in pixels
 * @param height Window height in pixels
 * @return true if initialization successful, false otherwise
 * 
 * This function sets up a procedural wave animation system:
 * - Compiles vertex and fragment shaders for wave displacement
 * - Sets up uniform locations for time-based animation control
 * - Configures depth testing for proper 3D wave rendering
 * - Initializes text overlay for sample identification
 * 
 * The wave effect uses vertex shader displacement where each vertex of a grid
 * is moved vertically based on mathematical wave functions (sine/cosine) and
 * the current time, creating realistic water wave motion entirely on the GPU.
 */
bool prepareGraphics(int width, int height) {
    string vertexShaderPath = resourceDirectory + vertexShaderFilename;
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    text = new FontEngine(fontResourceDirectory.c_str(), width, height);
    text->addString(0, 0, "RenderBlitzSDK: Waveform Sample", 255, 0, 0, 255);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    printf("vertexShaderID = %d", vertexShaderID);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
    printf("fragmentShaderID = %d", fragmentShaderID);

    programID = glCreateProgram();
    if (programID == 0) {
        printf("Could not create program.");
        return false;
    }

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);

    iLocPosition = glGetAttribLocation(programID, "a_v4Position");
    itimeLocation = glGetUniformLocation(programID, "iGlobalTime");
    iLocMVP = glGetUniformLocation(programID, "mvp");

    glViewport(0, 0, width, height);

    glClearColor(0.8f, 0.8f, 0.0f, 1.0f);
    glClearDepthf(1.0f);
    return true;
}

static void usage(void) {
    printf("Usage:\n");
    printf("  -Window <Window System name>  Set X11 or FBDEV\n");
    printf("  -info display OpenGL renderer info\n");
}

int main(int argc, char *argv[]) {

    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);

    prepareGraphics(WINDOW_W, WINDOW_H);

    bool end = false;
    float i = 0.0f;
    while (!end) {
        if (podium->checkWindow() != XPodium::WINDOW_IDLE) {
            end = true;
        }

        GraphicsUpdate(i);
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);

        if (i <= 10000)
            i = i + 0.04f;
        else
            i = 0;
    }
    delete text;
    CoreEGL::terminateEGL();
    podium->destroyWindow();
    return 0;
}
