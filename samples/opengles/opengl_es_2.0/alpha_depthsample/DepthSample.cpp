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
 * @file DepthSample.cpp
 *
 * This is an test example for OpenGL rendering using this SDK.
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
/******************************* Header Files ********************************/

#include "FontEngine.h"
#include "XEGLIntf.h"
#include "XGLSLCompile.h"
#include "XMatrixAPI.h"
#include "XPodium.h"
#include "XTexture.h"
#include "cstring"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

using namespace std;
using std::string;
using namespace SDKXilinx;
FontEngine *text;

std::string fontResourceDirectory = "FontShaders/";
#include "DepthSample.h"

bool supportFramebufferFetch = false;
GLuint fbo = 0;
GLint iLocViewportSize;
GLint iLocCubeDepthSampleTexture;

/**
 * @brief Graphics rendering function demonstrating alpha blending and depth testing
 * 
 * This function renders a complex scene with two objects demonstrating advanced OpenGL ES features:
 * - A rotating cube with depth testing
 * - A translucent glass plane with alpha blending and depth projection
 * - Supports both framebuffer fetch and render-to-texture approaches for depth sampling
 * 
 * The rendering pipeline:
 * 1. Renders the cube to establish depth information
 * 2. Renders the glass plane with transparency and depth-based effects
 * 3. Combines both objects with proper depth sorting and alpha blending
 */
void GraphicsUpdate(void) {
    // Setup and render the rotating cube
    glUseProgram(programCubeID);

    glEnableVertexAttribArray(iLocCubePosition);
    glEnableVertexAttribArray(iLocCubeColor);

    glVertexAttribPointer(iLocCubePosition, 3, GL_FLOAT, GL_FALSE, 0, verticesCube);
    glVertexAttribPointer(iLocCubeColor, 3, GL_FLOAT, GL_FALSE, 0, colorsCube);

    // Calculate cube rotation matrices for continuous animation
    static float angleY = 0;
    static float angleX = 0;
    XMatrixAPI modelView = XMatrixAPI::createRotationX(angleX);
    XMatrixAPI rotationY = XMatrixAPI::createRotationY(angleY);
    modelView = modelView * rotationY;

    // Setup camera view matrix
    CVec3f eyeVector = {0.3, 0.1, 2.5};      // Camera position
    CVec3f lookAtPoint = {0.0, -1.0, 0.0};   // Look-at target
    CVec3f upVector = {0.0, 1.0, 0.0};       // Up direction

    XMatrixAPI viewMatrix = XMatrixAPI::XMatrixAPICameraLookAt(eyeVector, lookAtPoint, upVector);
    modelView = viewMatrix * modelView;

    // Define depth and material parameters for glass effect
    const GLfloat fNear = 0.01;
    const GLfloat fFar = 3.0;
    const GLfloat glassMaterial = 1.8;

    // Create perspective projection matrix
    XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0, windowWidth / (float)windowHeight, fNear, fFar);
    XMatrixAPI modelViewPerspective = perspective * modelView;

    glUniformMatrix4fv(iLocCubeMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());

    // Update rotation angles for continuous animation
    angleY += 1;
    angleX += 1;

    // Keep angles within 0-360 degree range
    if (angleY >= 360)
        angleY -= 360;
    if (angleY < 0)
        angleY += 360;

    if (angleX >= 360)
        angleX -= 360;
    if (angleX < 0)
        angleX += 360;

    // Render cube using appropriate method based on hardware support
    if (supportFramebufferFetch) {
        // Direct rendering with framebuffer fetch extension
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    } else {
        // Render-to-texture approach for depth sampling
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Setup and render the translucent glass plane
    glUseProgram(programGlassID);
    glEnableVertexAttribArray(iLocGlassPosition);
    glEnableVertexAttribArray(iLocGlassColor);
    glVertexAttribPointer(iLocGlassPosition, 3, GL_FLOAT, GL_FALSE, 0, verticesGlass);
    glVertexAttribPointer(iLocGlassColor, 4, GL_FLOAT, GL_FALSE, 0, colorsGlass);
    
    // Position the glass plane in 3D space
    modelView = XMatrixAPI::createTranslation(0.5, 0.1, 0.8);
    modelView = viewMatrix * modelView;
    modelViewPerspective = perspective * modelView;
    glUniformMatrix4fv(iLocGlassMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());
    
    // Set shader uniforms for depth projection and material properties
    glUniform1f(iLocNear, fNear);
    glUniform1f(iLocFar, fFar);
    glUniform1f(iLocGlassMaterial, glassMaterial);

    // Set additional uniforms for render-to-texture method
    if (!supportFramebufferFetch) {
        glUniform2f(iLocViewportSize, (float)windowWidth, (float)windowHeight);
        glUniform1i(iLocCubeDepthSampleTexture, 1);
    }

    // Render the glass plane (4 vertices as triangle strip)
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // Render text overlay with sample information
    text->draw();
}

/**
 * @brief Initializes graphics resources for alpha blending and depth projection demo
 * 
 * @param width Window width in pixels
 * @param height Window height in pixels
 * @return true if initialization successful, false otherwise
 * 
 * This function sets up a complex rendering pipeline that demonstrates:
 * - Alpha blending with proper depth testing
 * - Framebuffer objects for depth texture sampling
 * - Multiple shader programs for different rendering passes
 * - Extension checking for hardware-specific optimizations
 * 
 * The function creates two shader programs:
 * 1. Cube shader for solid object rendering
 * 2. Glass shader for translucent object with depth projection effects
 * 
 * It also handles two rendering paths:
 * - ARM framebuffer fetch extension for direct depth access
 * - Standard render-to-texture for depth sampling compatibility
 */
bool prepareGraphics(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    // Setup shader file paths for both rendering objects
    string vertexShaderCubePath = resourceDirectory + vertexShaderCubeFilename;
    string fragmentShaderCubePath = resourceDirectory + fragmentShaderCubeFilename;
    string vertexShaderGlassPath = resourceDirectory + vertexShaderGlassFilename;
    string fragmentShaderGlassPath;

    // Choose appropriate glass fragment shader based on hardware support
    if (supportFramebufferFetch) {
        fragmentShaderGlassPath = resourceDirectory + fragmentShaderGlassFilename;
    } else {
        // Use render-to-texture version for compatibility
        fragmentShaderGlassPath = resourceDirectory + fragmentShaderGlassRttFilename;
        
        // Create framebuffer object for depth texture sampling
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        GLuint textureId;
        glGenTextures(1, &textureId);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureId);
        
        // Configure depth texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);

        // Verify framebuffer completeness
        if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
            printf("glCheckFramebufferStatue failed\n");
        }
    }

    // Enable alpha blending for transparency effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize font engine for text overlay
    text = new FontEngine(fontResourceDirectory.c_str(), width, height);
    text->addString(0, 0, "RenderBlitzSDK: Depth Projection Sample", 255, 0, 0, 255);

    // Compile all required shaders
    GLuint fragmentShaderCubeID, vertexShaderCubeID;
    GLuint fragmentShaderGlassID, vertexShaderGlassID;
    Shader::processShader(&vertexShaderCubeID, vertexShaderCubePath.c_str(), GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderCubeID, fragmentShaderCubePath.c_str(), GL_FRAGMENT_SHADER);
    Shader::processShader(&vertexShaderGlassID, vertexShaderGlassPath.c_str(), GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderGlassID, fragmentShaderGlassPath.c_str(), GL_FRAGMENT_SHADER);

    // Create and link shader programs
    programCubeID = glCreateProgram();
    programGlassID = glCreateProgram();

    glAttachShader(programCubeID, vertexShaderCubeID);
    glAttachShader(programCubeID, fragmentShaderCubeID);
    glLinkProgram(programCubeID);
    glAttachShader(programGlassID, vertexShaderGlassID);
    glAttachShader(programGlassID, fragmentShaderGlassID);
    glLinkProgram(programGlassID);

    // Get attribute and uniform locations for cube rendering
    iLocCubePosition = glGetAttribLocation(programCubeID, "av4position");
    iLocCubeColor = glGetAttribLocation(programCubeID, "av3color");
    iLocCubeMVP = glGetUniformLocation(programCubeID, "mvp");

    // Get attribute and uniform locations for glass rendering
    iLocGlassPosition = glGetAttribLocation(programGlassID, "av4position");
    iLocGlassColor = glGetAttribLocation(programGlassID, "av4color");

    iLocGlassMVP = glGetUniformLocation(programGlassID, "mvp");
    iLocNear = glGetUniformLocation(programGlassID, "near");
    iLocFar = glGetUniformLocation(programGlassID, "far");
    iLocGlassMaterial = glGetUniformLocation(programGlassID, "glassMaterial");

    // Get additional uniforms for render-to-texture method
    if (!supportFramebufferFetch) {
        iLocViewportSize = glGetUniformLocation(programGlassID, "viewportSize");
        iLocCubeDepthSampleTexture = glGetUniformLocation(programGlassID, "cubeDepthSampleTexture");
    }

    glUseProgram(programCubeID);

    // Enable required OpenGL features for proper 3D rendering
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Set clear color (yellow background)
    glClearColor(0.8f, 0.8f, 0.0f, 1.0f);

    return true;
}

/**
 * @brief Main application entry point for Alpha Depth demonstration
 * 
 * @return Exit status code (0 for success, 1 for extension failure)
 * 
 * This function demonstrates advanced OpenGL ES features including:
 * - Hardware extension detection and fallback handling
 * - Alpha blending with depth projection effects
 * - Framebuffer fetch vs render-to-texture comparison
 * 
 * The application checks for two possible rendering paths:
 * 1. ARM_shader_framebuffer_fetch_depth_stencil (preferred for performance)
 * 2. OES_depth_texture (fallback for compatibility)
 * 
 * If neither extension is available, the application exits with an error.
 * The sample showcases how to handle hardware differences gracefully
 * while maintaining visual fidelity across different GPU implementations.
 */
int main(void) {

    // Initialize windowing system
    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    
    // Setup EGL context for OpenGL ES 2.0
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);
    
    // Check for required OpenGL ES extensions
    const char *extensions = (const char *)glGetString(GL_EXTENSIONS);
    if (std::strstr(extensions, "GL_ARM_shader_framebuffer_fetch_depth_stencil")) {
        // Preferred: ARM framebuffer fetch extension for direct depth access
        supportFramebufferFetch = true;
    } else if (!std::strstr(extensions, "GL_OES_depth_texture")) {
        // Error: Neither extension is available
        printf("OpenGL ES 2.0 implementation neither support ARM_shader_framebuffer_fetch_depth_stencil nor support "
               "OES_depth_texture extension.");
        exit(1);
    }

    // Initialize graphics resources based on available extensions
    prepareGraphics(WINDOW_W, WINDOW_H);

    bool end = false;
    
    // Main rendering loop
    while (!end) {
        // Check for window events (close, minimize, etc.)
        if (podium->checkWindow() != XPodium::WINDOW_IDLE) {
            end = true;
        }

        // Render the complete alpha depth demonstration scene
        GraphicsUpdate();
        
        // Present the rendered frame to display
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }
    
    // Clean up allocated resources
    delete text;
    CoreEGL::terminateEGL();
    podium->destroyWindow();

    return 0;
}
