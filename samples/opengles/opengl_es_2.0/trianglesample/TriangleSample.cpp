/*******************************************************************************
 *
 * Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
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
 * @file TriangleSample.cpp
 *
 * This file implements test application for OpenGL rloopering using SDK
 *
 * @note        None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        -----------------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 3.0   Nirmal S       07/09/25        Added Wayland Support + FPS Timer + Bug Fixes
 * </pre>
 *
*******************************************************************************/
/******************************* Source Files ********************************/


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include "FontEngine.h"
#include "TriangleSample.h"
#include "XEGLIntf.h"
#include "XGLSLCompile.h"
#include "XMatrixAPI.h"
#include "XPodium.h"
#include "XTime.h"

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif


using std::string;
using namespace SDKXilinx;

/** @brief Text rendering engine for displaying FPS and other information */
FontEngine* text;

/** @brief FPS display string updated each frame */
static string fpsString = "FPS: --.-";

/**
 * @brief Initialize OpenGL ES graphics subsystem and prepare shaders
 * 
 * This function sets up the OpenGL ES 2.0 rendering context, compiles and links
 * the vertex and fragment shaders, initializes the font engine, and configures
 * the rendering pipeline for triangle drawing.
 * 
 * @param width  Width of the rendering viewport in pixels
 * @param height Height of the rendering viewport in pixels
 * @return true if initialization successful, false otherwise
 * 
 * @note Sets up depth testing, alpha blending, and viewport transformation
 */
bool prepareGraphics(int width, int height)
{
    printf("prepareGraphics(%d, %d)", width, height);
    string vertexShaderPath = resourceDirectory + vertexShaderFilename; 
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Initialize the FontEngine object and add some text. */
    text = new FontEngine(fontResourceDirectory.c_str(), width, height);

    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    printf("vertexShaderID = %d", vertexShaderID);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
    printf("fragmentShaderID = %d", fragmentShaderID);

    programID = glCreateProgram();
    if (programID == 0)
    {
        printf("Could not create program.");
        return false;
    }

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);

    iLocPosition = glGetAttribLocation(programID, "a_v4Position");
    printf("glGetAttribLocation(\"a_v4Position\") = %d\n", iLocPosition);

    iLocFillColor = glGetAttribLocation(programID, "a_v4FillColor");
    printf("glGetAttribLocation(\"a_v4FillColor\") = %d\n", iLocFillColor);

    iLocMVP = glGetUniformLocation(programID, "mvp");

    glViewport(0, 0, width, height);

    glClearColor(0.8f, 0.8f, 0.0f, 1.0f);
    glClearDepthf(1.0f);
    return true;
}

/**
 * @brief Main rendering function called each frame
 * 
 * This function performs the following operations:
 * - Clears color and depth buffers
 * - Sets up vertex attribute pointers for position and color
 * - Calculates and applies model-view-projection matrix transformations
 * - Renders the triangle with rotation animation
 * - Draws text overlay using FontEngine
 * 
 * The triangle is rotated around the Y-axis at 1 degree per frame,
 * creating a continuous spinning animation effect.
 */
void GraphicsUpdate(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, triangleVertices);

    glEnableVertexAttribArray(iLocPosition);

    if(iLocFillColor != -1)
    {
        glVertexAttribPointer(iLocFillColor, 4, GL_FLOAT, GL_FALSE, 0, triangleColors);
        glEnableVertexAttribArray(iLocFillColor);
    }

    // Animation rotation angles for X, Y, and Z axes
	static float rotateangleX = 0, rotateangleY = 0, rotateangleZ = 0;
        
        // Create rotation matrices for each axis
        XMatrixAPI modelView = XMatrixAPI::createRotationX(rotateangleX);
        XMatrixAPI rotation = XMatrixAPI::createRotationY(rotateangleY);

        // Combine rotations: Y * X
        modelView = rotation * modelView;

        // Add Z rotation and translation
        rotation = XMatrixAPI::createRotationZ(rotateangleZ);
        XMatrixAPI translate1 = XMatrixAPI::createTranslation(0.0,0.0,0.0);

        // Final model-view matrix: Translation * Z-rotation * Y-rotation * X-rotation
        modelView =  translate1 *rotation * modelView;

        // Move triangle away from camera (Z = -1.5)
        modelView[14] -= 1.5;

        // Create perspective projection matrix (45° FOV, 16:9 aspect ratio)
        XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, 16 / 9, 0.01f, 100.0f);
        
        // Combine perspective and model-view for final MVP matrix
        XMatrixAPI modelViewPerspective = perspective * modelView;

        // Upload MVP matrix to vertex shader uniform
        glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());
        
        // Update rotation angles for next frame (Y-axis rotation creates spinning effect)
        rotateangleX += 0;
        rotateangleY -= 1;  // Rotate 1 degree per frame around Y-axis
        rotateangleZ += 0;

        // Keep rotation angles in [0, 360) range for numerical stability
        if(rotateangleX >= 360) rotateangleX -= 360;
        if(rotateangleX < 0) rotateangleX += 360;
        if(rotateangleY >= 360) rotateangleY -= 360;
        if(rotateangleY < 0) rotateangleY += 360;
        if(rotateangleZ >= 360) rotateangleZ -= 360;
        if(rotateangleZ < 0) rotateangleZ += 360;
        
        // Draw the triangle (3 vertices forming 1 triangle)
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
	/* Draw fonts. */
	text->draw();

}


int main(int argc, char* argv[])
{
    // Initialize cross-platform windowing system
    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);

     /* If we were passed an argument, request that as EGL_SAMPLES. */
    if(argc >= 2)
    {
        int samples = atoi(argv[1]);
        CoreEGL::setEGLSamples(samples);
    }

    // Initialize EGL context for OpenGL ES 2.0
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);

     /* Recover the actual EGL_SAMPLE value. */
    int attributeValue = -1;
    eglGetConfigAttrib(CoreEGL::display, CoreEGL::config, EGL_SAMPLES, &attributeValue);
    printf("EGL_SAMPLES = %d", attributeValue);

    // Initialize graphics resources and shaders
    prepareGraphics(WINDOW_W, WINDOW_H);

    // Initialize FPS timing
    Timer fpsTimer;
    fpsTimer.reset();

    // Main rendering loop
    bool loop = false;
    while(!loop)
    {
        // Check for window close event
        if(podium->checkWindow() != XPodium::WINDOW_IDLE)
        {
            loop = true;
        }
        
        // Clear and set base text
        text->addString(10, 0, "RenderBlitzSDK: Anti-Aliased Triangle Sample", 255, 0, 0, 255);

        // Render the animated triangle
        GraphicsUpdate();
        
        // Present the rendered frame
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);

        // Measure timing after the frame is complete
        float FPS = fpsTimer.getFPS();
        
        // Update FPS string every frame for smooth display
        std::ostringstream fpsStream;
        fpsStream << "FPS: " << std::fixed << std::setprecision(1) << FPS;
        fpsString = fpsStream.str();
        
        // Add FPS text to screen
        text->clear();
        text->addString(10, 20, fpsString.c_str(), 255, 0, 0, 255);
        
        // Print FPS to terminal every second
        if(fpsTimer.isTimePassed(1.0f))
        {
            printf("FPS: %.1f\n", FPS);
        }
    }

    /* Shut down FontEngine. */
    delete text;
    
    // Clean up EGL and windowing resources
    CoreEGL::terminateEGL();
    podium->destroyWindow();

    return 0;
}

/**
 * @brief Main application entry point
 * 
 * Initializes the windowing system, EGL context, OpenGL ES graphics,
 * and runs the main rendering loop until the user closes the window.
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 *             argv[1] (optional): Number of MSAA samples to request
 * @return 0 on successful execution, non-zero on error
 * 
 * The main loop:
 * 1. Checks for window close events
 * 2. Updates and displays FPS information
 * 3. Renders the animated triangle
 * 4. Swaps front/back buffers for smooth animation
 */
