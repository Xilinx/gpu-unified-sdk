/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

#include "XPodium.h"
#include "FontEngine.h"
#include "XGLSLCompile.h"
#include "XMatrixAPI.h"
#include "XGLSLCompile.h"
#include "XTime.h"
#include "XEGLIntf.h"
#include "EGLpreserve.h"

#include <string>

#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

using std::string;
using namespace SDKXilinx;

/* Asset directories and filenames. */
string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "EGLPreserve_cube.vert";
string fragmentShaderFilename = "EGLPreserve_cube.frag";

string scissorOff = "Scissor:  off";
string scissorOn = "Scissor:  on ";
string preserveOff = "Preserve: off";
string preserveOn = "Preserve: on ";

/* ShaderEngine variables. */
GLuint programID = 0;
GLint iLocPosition = -1;
GLint iLocFillColor = -1;
GLint iLocProjection = -1;
GLint iLocModelview = -1;

/* Animation variables. */
XMatrixAPI perspective;
XMatrixAPI translation;

unsigned int windowWidth = 0;
unsigned int windowHeight = 0;

/* ChronoEngine variable to handle the animation timing. */
Timer animationTimer;

/* A text object to draw text on the screen. */
FontEngine* text;

bool prepareGraphics(int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    /* Full paths to the shader and texture files */
    string vertexShaderPath = resourceDirectory + vertexShaderFilename;
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    /* Initialize matrices. */
    perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, windowWidth / (float)windowHeight, 0.01f, 100.0f);
    /* Move cube 2 further away from camera. */
    translation = XMatrixAPI::createTranslation(0.0f, 0.0f, -2.0f);

    /* Initialize OpenGL ES. */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    /* Should do src * (src alpha) + dest * (1-src alpha). */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Initialize the FontEngine object. */
    text = new FontEngine(resourceDirectory.c_str(), windowWidth, windowHeight);

    /* Process shaders. */
    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;
    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);

    /* Set up shaders. */
    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);

    /* Vertex positions. */
    iLocPosition = glGetAttribLocation(programID, "a_v4Position");
    if (iLocPosition == -1)
    {
        printf("Attribute not found at %s:%i\n", __FILE__, __LINE__);
        return false;
    }
    glEnableVertexAttribArray(iLocPosition);

    /* Vertex colors. */
    iLocFillColor = glGetAttribLocation(programID, "a_v4FillColor");
    if (iLocFillColor == -1)
    {
        printf("Warning: Attribute not found at %s:%i\n", __FILE__, __LINE__);
    }
    else
    {
        glEnableVertexAttribArray(iLocFillColor);
    }

    /* Projection matrix. */
    iLocProjection = glGetUniformLocation(programID, "u_m4Projection");
    if (iLocProjection == -1)
    {
        printf("Warning: Uniform not found at %s:%i\n", __FILE__, __LINE__);
    }
    else
    {
        glUniformMatrix4fv(iLocProjection, 1, GL_FALSE, perspective.getAsArray());
    }

    /* Modelview matrix. */
    iLocModelview = glGetUniformLocation(programID, "u_m4Modelview");
    if (iLocModelview == -1)
    {
        printf("Warning: Uniform not found at %s:%i\n", __FILE__, __LINE__);
    }

    /* Set clear screen color. */
    glClearColor(0.8f, 0.8f, 0.0f, 1.0f);

    text->addString(windowWidth - (unsigned int)scissorOff.length() * FontEngine::textureCharacterWidth, FontEngine::textureCharacterHeight, scissorOff.c_str(), 255, 0, 0, 255);

    /* Tweak EGL. */
    /* Check if the EGL_SWAP_BEHAVIOR_PRESERVED_BIT is set in EGL_SURFACE_TYPE. */
    EGLint swapBehaviour;
    if(eglQuerySurface(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW), EGL_SWAP_BEHAVIOR, &swapBehaviour) != EGL_TRUE)
    {
    }

    switch(swapBehaviour)
    {
        case (EGL_BUFFER_PRESERVED):
            break;
        case (EGL_BUFFER_DESTROYED):
            break;
        default:
            break;
    }

    /* Set preserve bit. */
    if(eglSurfaceAttrib(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW), EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED) != EGL_TRUE)
    {
    }
    return true;
}

void renderFrame(void)
{
    static float angleX = 0.0f;
    static float angleY = 0.0f;
    static float angleZ = 0.0f;

    static bool fullScreen = false;
    static bool preserve = false;

    /* Change the animation state if more than 3 seconds are passed since the last state change. */
    if(animationTimer.isTimePassed(3.0f))
    {
        fullScreen = !fullScreen;

        text->clear();
        if(fullScreen)
        {
            text->addString(windowWidth - (unsigned int)scissorOff.length() * FontEngine::textureCharacterWidth, FontEngine::textureCharacterHeight, scissorOff.c_str(), 255, 0, 0, 255);

            glDisable(GL_SCISSOR_TEST);
            preserve = !preserve;
            if(preserve)
            {
                /* Set preserve bit. */
                if(eglSurfaceAttrib(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW), EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED) != EGL_TRUE)
                {
                }
            }
            else
            {
                /* Clear preserve bit. */
                if(eglSurfaceAttrib(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW), EGL_SWAP_BEHAVIOR, EGL_BUFFER_DESTROYED) != EGL_TRUE)
                {
                }
            }
        }
        else
        {
            text->addString(windowWidth - (unsigned int)scissorOn.length() * FontEngine::textureCharacterWidth, FontEngine::textureCharacterHeight, scissorOn.c_str(), 0, 255, 0, 255);
            glEnable(GL_SCISSOR_TEST);
            glScissor(windowWidth / 2, 0, windowWidth / 2, windowHeight);
            if(preserve)
            {
                text->addString(windowWidth - (unsigned int)preserveOn.length() * FontEngine::textureCharacterWidth, 0, preserveOn.c_str(), 0, 255, 0, 255);
            }
            else
            {
                text->addString(windowWidth - (unsigned int)preserveOff.length() * FontEngine::textureCharacterWidth, 0, preserveOff.c_str(), 255, 0, 0, 255);
            }
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    XMatrixAPI rotationX = XMatrixAPI::createRotationX(angleX);
    XMatrixAPI rotationY = XMatrixAPI::createRotationY(angleY);
    XMatrixAPI rotationZ = XMatrixAPI::createRotationZ(angleZ);
    /* Update cube's rotation angles for animating. */
    angleX += 3;
    angleY += 2;
    angleZ += 1;

    if (angleX >= 360) angleX -= 360;
    if (angleY >= 360) angleY -= 360;
    if (angleZ >= 360) angleZ -= 360;

    /* Rotate about origin, then translate away from camera. */
    XMatrixAPI modelView = translation * rotationX;
    modelView = modelView * rotationY;
    modelView = modelView * rotationZ;

    glUseProgram(programID);

    glUniformMatrix4fv(iLocModelview, 1, GL_FALSE, modelView.getAsArray());
    glUniformMatrix4fv(iLocProjection, 1, GL_FALSE, perspective.getAsArray());

    glEnableVertexAttribArray(iLocPosition);
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, cubeVertices);

    if (iLocFillColor != -1)
    {
        glEnableVertexAttribArray(iLocFillColor);
        glVertexAttribPointer(iLocFillColor, 4, GL_FLOAT, GL_FALSE, 0, cubeColors);
    }

    /* Draw the cube. */
    glDrawElements(GL_TRIANGLE_STRIP, 22, GL_UNSIGNED_BYTE, cubeIndices);

    /* Draw fonts. */
    text->draw();
}

/**
 * @brief Main application entry point for EGL buffer preservation demonstration
 * 
 * @return Exit status code (0 for success)
 * 
 * This application demonstrates EGL_SWAP_BEHAVIOR_PRESERVED_BIT functionality:
 * - Tests EGL buffer preservation behavior during surface swaps
 * - Shows difference between preserved and destroyed buffer contents
 * - Demonstrates proper EGL context and surface management
 * - Illustrates performance implications of buffer preservation
 * 
 * Buffer preservation allows applications to:
 * - Maintain frame buffer contents between swaps
 * - Implement incremental rendering strategies
 * - Optimize performance for partial screen updates
 * - Handle complex rendering scenarios requiring multi-frame operations
 * 
 * This sample is crucial for understanding EGL behavior and optimizing
 * applications that need fine control over frame buffer management,
 * particularly useful for UI applications and overlay systems.
 */
int main(void)
{
    /* Initialize the platform-specific functions. */
    XPodium* podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);

    /* Initialize EGL. */
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);

    /* Initialize OpenGL ES graphics subsystem. */
    prepareGraphics(WINDOW_W, WINDOW_H);

    bool loop = false;
    /* The rendering loop to draw the scene. */
    while (!loop)
    {
        if (podium->checkWindow() != XPodium::WINDOW_IDLE)
        {
            loop = true;
        }

        renderFrame();

        /* Push the EGL surface color buffer to the native window. */
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }

    /* Shut down FontEngine and EGL. */
    delete text;
    CoreEGL::terminateEGL();

    /* Destroy the windowing system. */
    podium->destroyWindow();

    return 0;
}

