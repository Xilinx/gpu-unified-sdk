/* Copyright (c) 2012-2017, ARM Limited and Contributors
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
 * Modifications Copyright (C) 2024-2025 Advanced Micro Devices, Inc.  All rights reserved.
 */

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "FontEngine.h"
#include "RotoZoom.h"
#include "XEGLIntf.h"
#include "XGLSLCompile.h"
#include "XMathematics.h"
#include "XMatrixAPI.h"
#include "XPodium.h"
#include "XTexture.h"

#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

using std::string;
using namespace SDKXilinx;

FontEngine *text;

#define GL_CHECK(x)                                                                                                    \
    x;                                                                                                                 \
    {                                                                                                                  \
        GLenum glError = glGetError();                                                                                 \
        if (glError != GL_NO_ERROR) {                                                                                  \
            printf("glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);                     \
            exit(1);                                                                                                   \
        }                                                                                                              \
    }

/* Asset directories and filenames. */
std::string fontResourceDirectory = "FontShaders/";
string resourceDirectory = "Supportingfiles/";
string textureFilename = "kria_RotoZoom.raw";
string vertexShaderFilename = "RotoZoom_cube.vert";
string fragmentShaderFilename = "RotoZoom_cube.frag";

/* TextureEngine variables. */
GLuint textureID = 0;

/* ShaderEngine variables. */
GLuint programID = 0;
GLint iLocTextureMatrix = -1;
GLint iLocPosition = -1;
GLint iLocTextureMix = -1;
GLint iLocTexture = -1;
GLint iLocTexCoord = -1;

/* Animation variables. */
XMatrixAPI translation;
XMatrixAPI scale;
XMatrixAPI negativeTranslation;

int windowWidth = -1;
int windowHeight = -1;

/* A text object to draw text on the screen. */

bool setupGraphics(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    string texturePath = resourceDirectory + textureFilename;
    string vertexShaderPath = resourceDirectory + vertexShaderFilename;
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    translation = XMatrixAPI::createTranslation(0.5f, 0.5f, 0.0f);
    scale = XMatrixAPI::createScaling(width / 256.0f, height / 256.0f, 1.0f);
    negativeTranslation = XMatrixAPI::createTranslation(-0.5f, -0.5f, 0.0f);

    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glCullFace(GL_BACK));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    text = new FontEngine(fontResourceDirectory.c_str(), width, height);
    text->addString(0, 0, "RenderBlitzSDK: Rotozoom Sample", 255, 0, 0, 255);

    GL_CHECK(glGenTextures(1, &textureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
    unsigned char *textureData = NULL;
    XTexture::XloadData(texturePath.c_str(), &textureData);
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 844, 398, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData));

    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;
    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);

    programID = GL_CHECK(glCreateProgram());
    GL_CHECK(glAttachShader(programID, vertexShaderID));
    GL_CHECK(glAttachShader(programID, fragmentShaderID));
    GL_CHECK(glLinkProgram(programID));
    GL_CHECK(glUseProgram(programID));

    iLocPosition = GL_CHECK(glGetAttribLocation(programID, "a_v4Position"));
    if (iLocPosition == -1) {
        return false;
    }
    GL_CHECK(glEnableVertexAttribArray(iLocPosition));

    iLocTexture = GL_CHECK(glGetUniformLocation(programID, "u_s2dTexture"));
    if (iLocTexture == -1) {
    } else {
        GL_CHECK(glUniform1i(iLocTexture, 0));
    }

    iLocTexCoord = GL_CHECK(glGetAttribLocation(programID, "a_v2TexCoord"));
    if (iLocTexCoord == -1) {
    } else {
        GL_CHECK(glEnableVertexAttribArray(iLocTexCoord));
    }

    iLocTextureMatrix = GL_CHECK(glGetUniformLocation(programID, "u_m4Texture"));
    if (iLocTextureMatrix == -1) {
    } else {
        GL_CHECK(glUniformMatrix4fv(iLocTextureMatrix, 1, GL_FALSE, scale.getAsArray()));
    }

    return true;
}

void renderFrame(void) {
    static float angleZTexture = 0.0f;
    static float angleZOffset = 0.0f;
    static float angleZoom = 0.0f;
    CVec4f radius = {0.0f, 2.0f, 0.0f, 1.0f};

    GL_CHECK(glUseProgram(programID));
    GL_CHECK(glEnableVertexAttribArray(iLocPosition));
    GL_CHECK(glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, quadVertices));
    if (iLocTexCoord != -1) {
        GL_CHECK(glEnableVertexAttribArray(iLocTexCoord));
        GL_CHECK(glVertexAttribPointer(iLocTexCoord, 2, GL_FLOAT, GL_FALSE, 0, quadTextureCoordinates));
    }
    GL_CHECK(glViewport(0, 0, windowWidth, windowHeight));
    GL_CHECK(glClearColor(0.8f, 0.8f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    XMatrixAPI rotateTextureZ = XMatrixAPI::createRotationZ(angleZTexture);
    XMatrixAPI rotateOffsetZ = XMatrixAPI::createRotationZ(angleZOffset);
    CVec4f offset = XMatrixAPI::vertexTransform(&radius, &rotateOffsetZ);
    XMatrixAPI translateTexture = XMatrixAPI::createTranslation(offset.x, offset.y, offset.z);
    XMatrixAPI zoom = XMatrixAPI::createScaling(sinf(degreesToRadians(angleZoom)) * 0.75f + 1.25f,
                                                sinf(degreesToRadians(angleZoom)) * 0.75f + 1.25f, 1.0f);
    XMatrixAPI textureMovement = XMatrixAPI::identityXMatrixAPI * translation;
    textureMovement = textureMovement * rotateTextureZ;
    textureMovement = textureMovement * translateTexture;
    textureMovement = textureMovement * zoom;
    textureMovement = textureMovement * scale;
    textureMovement = textureMovement * negativeTranslation;
    GL_CHECK(glUniformMatrix4fv(iLocTextureMatrix, 1, GL_FALSE, textureMovement.getAsArray()));

    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
    GL_CHECK(glDrawElements(GL_TRIANGLE_STRIP, sizeof(quadIndices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, quadIndices));

    angleZTexture -= 1;
    angleZOffset -= 1;
    angleZoom -= 1;
    if (angleZTexture >= 360)
        angleZTexture -= 360;
    if (angleZTexture < 0)
        angleZTexture += 360;
    if (angleZOffset >= 360)
        angleZOffset -= 360;
    if (angleZOffset < 0)
        angleZOffset += 360;
    if (angleZoom >= 360)
        angleZoom -= 360;
    if (angleZoom < 0)
        angleZoom += 360;
    text->draw();
}

/**
 * @brief Main application entry point for RotoZoom demonstration
 * 
 * @return Exit status code (0 for success)
 * 
 * This application demonstrates classic demoscene RotoZoom effects using OpenGL ES:
 * - Real-time texture rotation and scaling transformations
 * - Smooth animation loops with sine/cosine-based movement
 * - GPU-accelerated texture sampling and interpolation
 * - Classic demo effects popularized in 1990s computer graphics
 * 
 * The RotoZoom effect combines:
 * - Rotation: Continuous texture rotation around center point
 * - Zoom: Periodic scaling in/out using trigonometric functions
 * - Translation: Offset movement creating tunnel-like effects
 * 
 * This technique demonstrates fundamental 2D graphics programming concepts
 * and is often used in games, demos, and retro-style applications for
 * creating engaging visual effects with minimal computational overhead.
 */
int main(void) {
    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);
    setupGraphics(WINDOW_W, WINDOW_H);

    bool end = false;
    while (!end) {
        if (podium->checkWindow() != XPodium::WINDOW_IDLE) {
            end = true;
        }

        renderFrame();
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }

    GL_CHECK(glDeleteTextures(1, &textureID));
    CoreEGL::terminateEGL();
    podium->destroyWindow();
    delete text;

    return 0;
}
