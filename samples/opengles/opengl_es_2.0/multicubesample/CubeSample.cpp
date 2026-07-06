/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/******************************************************************************/
/**
 *
 * @file CubeSample.cpp
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
#include "CubeSample.h"
#include "FontEngine.h"
#include "XEGLIntf.h"
#include "XGLSLCompile.h"
#include "XMatrixAPI.h"
#include "XPodium.h"
#include "XTime.h"


#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

using std::string;
using namespace std;
using namespace SDKAMD;

/** @brief Text rendering engine for displaying FPS and sample information */
FontEngine* text;

/** @brief FPS display string updated each frame */
static string fpsString = "FPS: --.-";

/**
 * @brief Initialize OpenGL ES graphics subsystem for multi-cube rendering
 * 
 * Sets up the OpenGL ES 2.0 rendering context with depth testing enabled,
 * compiles and links vertex/fragment shaders, initializes font rendering,
 * and configures the graphics pipeline for 3D cube rendering.
 * 
 * @param width  Width of the rendering viewport in pixels
 * @param height Height of the rendering viewport in pixels
 * @return true if initialization successful, false otherwise
 * 
 * @note Enables depth testing, alpha blending, and sets up shader program
 */
bool prepareGraphics(int width, int height)
{
    printf("prepareGraphics(%d, %d)\n", width, height);

    string vertexShaderPath = resourceDirectory + vertexShaderFilename; 
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	text = new FontEngine(fontResourceDirectory.c_str(), width, height);
	
    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
 
    programID = glCreateProgram();
    if (programID == 0)
    {
        printf("Could not create program.\n");
        return false;
    }

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);

    iLocPosition = glGetAttribLocation(programID, "a_v4Position");
   
    iLocFillColor = glGetAttribLocation(programID, "a_v4FillColor");
   
    iLocMVP = glGetUniformLocation(programID, "mvp");

    glViewport(0, 0, width, height);

	glClearColor(0.8f, 0.8f, 0.0f, 1.0f);
    glClearDepthf(1.0f);
    return true;
}

/**
 * @brief Graphics rendering function called every frame
 * 
 * This function performs the complete rendering cycle for two rotating cubes:
 * - Clears color and depth buffers
 * - Sets up vertex attributes for position and color
 * - Renders first cube with one rotation pattern and translation
 * - Renders second cube with different rotation pattern and translation
 * - Updates rotation angles for continuous animation
 * - Renders text overlay with SDK information and FPS
 */
void GraphicsUpdate(void)
{
    // Clear both color and depth buffers for new frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);
    
    // Set up vertex attribute pointers for cube geometry
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, cubeVertices);
    glEnableVertexAttribArray(iLocPosition);

    // Set up color attributes if available in shader
    if(iLocFillColor != -1)
    {
        glVertexAttribPointer(iLocFillColor, 3, GL_FLOAT, GL_FALSE, 0, cubeColors);
        glEnableVertexAttribArray(iLocFillColor);
    }

    // First cube rendering with unique rotation pattern
	static float angleX = 0, angleY = 0, angleZ = 0;
        XMatrixAPI modelView = XMatrixAPI::createRotationX(angleX);  // X-axis rotation
        XMatrixAPI rotation = XMatrixAPI::createRotationY(angleY);   // Y-axis rotation
        modelView = rotation * modelView;
        rotation = XMatrixAPI::createRotationZ(angleZ);              // Z-axis rotation
        XMatrixAPI translate1 = XMatrixAPI::createTranslation(-1.0,0.0,0.0);  // Position left of center
        modelView =  translate1 *rotation * modelView;
        modelView[14] -= 4.5;  // Move cube back in Z-direction for visibility
        
        // Create perspective projection matrix
        XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, 16 / 9, 0.01f, 100.0f);
        XMatrixAPI modelViewPerspective = perspective * modelView;
        glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());
        
        // Update rotation angles for first cube (4,3,2 degrees per frame)
        angleX += 4;
        angleY += 3;
        angleZ += 2;
        
        // Keep angles within 0-360 degree range
        if(angleX >= 360) angleX -= 360;
        if(angleX < 0) angleX += 360;
        if(angleY >= 360) angleY -= 360;
        if(angleY < 0) angleY += 360;
        if(angleZ >= 360) angleZ -= 360;
        if(angleZ < 0) angleZ += 360;
        
    	glDrawArrays(GL_TRIANGLES, 0, 36);  // Render first cube (36 vertices for 12 triangles)

    // Second cube rendering with different rotation pattern
	static float angle1X = 0, angle1Y = 0, angle1Z = 0;
        XMatrixAPI modelView1 = XMatrixAPI::createRotationX(angle1X);  // X-axis rotation
        XMatrixAPI rotation1 = XMatrixAPI::createRotationY(angle1Y);    // Y-axis rotation
        modelView1 = rotation1 * modelView1;
        rotation1 = XMatrixAPI::createRotationZ(angle1Z);               // Z-axis rotation
        XMatrixAPI translate2 = XMatrixAPI::createTranslation(1.0,0.0,0.0);  // Position right of center
        modelView1 =  translate2 *rotation1 * modelView1;
        modelView1[14] -= 4.5;  // Move cube back in Z-direction for visibility
        
        // Create perspective projection matrix for second cube
        XMatrixAPI perspective1 = XMatrixAPI::XMatrixAPIPerspective(45.0f, 16 / 9, 0.01f, 100.0f);
        XMatrixAPI modelViewPerspective1 = perspective1 * modelView1;
        glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective1.getAsArray());
        
        // Update rotation angles for second cube (2,3,4 degrees per frame - different pattern)
        angle1X += 2;
        angle1Y += 3;
        angle1Z += 4;
        
        // Keep angles within 0-360 degree range
        if(angle1X >= 360) angle1X -= 360;
        if(angle1X < 0) angle1X += 360;
        if(angle1Y >= 360) angle1Y -= 360;
        if(angle1Y < 0) angle1Y += 360;
        if(angle1Z >= 360) angle1Z -= 360;
        if(angle1Z < 0) angle1Z += 360;
        
        glDrawArrays(GL_TRIANGLES, 0, 36);  // Render second cube (36 vertices for 12 triangles)
		
    // Render text overlay (SDK title and FPS information)
	text->draw();


}

/**
 * @brief Main application entry point
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Exit status code (0 for success)
 * 
 * This function:
 * - Initializes the windowing system (XPodium)
 * - Sets up EGL context for OpenGL ES 2.0
 * - Prepares graphics resources (shaders, buffers, etc.)
 * - Runs the main rendering loop with FPS monitoring
 * - Handles window events and user input
 * - Cleans up resources before exit
 */
int main(int argc, char* argv[])
{
    
    // Initialize windowing system with specified dimensions
    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    
    // Initialize EGL context for OpenGL ES 2.0 rendering
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);
    
    // Prepare all graphics resources (shaders, buffers, etc.)
    prepareGraphics(WINDOW_W, WINDOW_H);

    // Initialize FPS timing for performance monitoring
    Timer fpsTimer;
    fpsTimer.reset();

    bool end = false;
    
    // Main rendering loop - continues until window close event
    while(!end)
    {
        // Check for window events (close, minimize, etc.)
        if(podium->checkWindow() != XPodium::WINDOW_IDLE)
        {
            end = true;
        }

        // Set base text for SDK identification
        text->addString(10, 0, "RenderBlitzSDK: Multiple Cube Sample", 255, 0, 0, 255);

        // Perform complete frame rendering
        GraphicsUpdate();
        
        // Present the rendered frame to display
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);

        // Calculate current frames per second
        float FPS = fpsTimer.getFPS();
        
        // Update FPS display string with current value
        std::ostringstream fpsStream;
        fpsStream << "FPS: " << std::fixed << std::setprecision(1) << FPS;
        fpsString = fpsStream.str();
        
        // Clear previous text and add current FPS text to screen
        text->clear();
        text->addString(10, 20, fpsString.c_str(), 255, 0, 0, 255);
        
        // Print FPS to console every second for debugging
        if(fpsTimer.isTimePassed(1.0f))
        {
            printf("FPS: %.1f\n", FPS);
        }
    }
    
    // Clean up allocated resources
    delete text;
    CoreEGL::terminateEGL();
    podium->destroyWindow();

    return 0;
}


