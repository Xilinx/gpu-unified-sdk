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
 * @file MipMapSample.cpp
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


#define LOAD_MIPMAPS
#undef DISABLE_MIPMAPS

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "XPodium.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"
#include "XTexture.h"
#include <string>
#include "FontEngine.h"

using std::string;

using namespace SDKXilinx;


#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

#include "MipMapSample.h"

FontEngine* text;

/**
 * @brief Initializes graphics resources for texture mipmap demonstration
 * 
 * @param w Window width in pixels
 * @param h Window height in pixels
 * @return true if initialization successful, false otherwise
 * 
 * This function demonstrates texture mipmapping techniques in OpenGL ES:
 * - ETC1 compressed texture format support verification
 * - Multiple mipmap loading strategies (pre-generated vs auto-generated)
 * - Texture filtering configuration for mipmap usage
 * 
 * The function supports three different mipmap approaches via preprocessor directives:
 * 1. LOAD_MIPMAPS: Load pre-generated mipmap levels from files
 * 2. Auto-generated: Use glGenerateMipmap() for automatic mipmap creation
 * 3. DISABLE_MIPMAPS: Disable mipmapping for comparison purposes
 * 
 * ETC1 texture compression is required and verified during initialization.
 */
bool prepareGraphics(int w, int h)
{
    // Setup resource file paths
    string texturePath = resourceDirectory + textureFilename;
    string vertexShaderPath = resourceDirectory + vertexShaderFilename; 
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;
    
    // Enable 3D rendering features
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    
    // Initialize font engine for text overlay
	text = new FontEngine(fontResourceDirectory.c_str(), w, h);
    text->addString(0, 0, "AMD RenderBlitzSDK: Textured MipMap Sample", 255, 0, 0, 255);
    
    // Enable alpha blending for text transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Verify ETC1 texture compression support (required for this sample)
    if (!XTexture::XisETCSupported(true))
    {
        printf("ETC1 not supported");
        return false;
    }

#ifdef LOAD_MIPMAPS
    // Method 1: Load pre-generated mipmap levels from separate files
    XTexture::XloadCompressedMipmaps(texturePath.c_str(), imageExtension.c_str(), &textureID);

    // Set explicit, well-defined sampler state: clamp the edges and use
    // trilinear filtering to actually exercise the loaded mipmap chain.
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else /* LOAD_MIPMAPS */
    // Method 2: Load base texture and either generate or disable mipmaps
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Load the base texture (mipmap level 0)
    string mainTexturePath = texturePath + "0" + imageExtension;
    unsigned char *textureData;
    Texture::loadData(mainTexturePath.c_str(), &textureData);
    ETCHeaderforTexture loadedETCHeaderforTexture = ETCHeaderforTexture(textureData);
    
    // Upload compressed ETC1 texture data to GPU
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES,
             loadedETCHeaderforTexture.getWidth(), loadedETCHeaderforTexture.getHeight(), 0,
             loadedETCHeaderforTexture.getPaddedWidth() * loadedETCHeaderforTexture.getPaddedHeight() >> 1,
             textureData + 16);
    free(textureData);

#    ifdef DISABLE_MIPMAPS
    // Option 2a: Disable mipmapping (use nearest neighbor filtering)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#    else /* DISABLE_MIPMAPS */
    // Option 2b: Auto-generate mipmap levels from base texture
    glGenerateMipmap(GL_TEXTURE_2D);
#    endif /* DISABLE_MIPMAPS */
#endif /* LOAD_MIPMAPS */

    // Compile and link shaders
    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    printf("vertexShaderID = %d", vertexShaderID);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
    printf("fragmentShaderID = %d", fragmentShaderID);
    
    // Create shader program and link shaders
    programID = glCreateProgram();
    if (!programID) 
    {
        printf("Could not create program.");
        return false;
    }
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);
    
    // Get vertex position attribute location
    iLocPosition = glGetAttribLocation(programID, "a_v4Position");
    if(iLocPosition == -1)
    {
        exit(1);
    }
    glEnableVertexAttribArray(iLocPosition);

    // Get texture coordinate attribute location
    iLocTexCoord = glGetAttribLocation(programID, "a_v2TexCoord");
    if(iLocTexCoord == -1)
    {
        printf("Warning: Attribute not found: \"a_v2TexCoord\"");
    }
    else
    {
        glEnableVertexAttribArray(iLocTexCoord);
    }

    // Get texture sampler uniform location
    iLocSampler = glGetUniformLocation(programID, "u_s2dTexture");
    if(iLocSampler == -1)
    {
        printf("Warning: Uniform not found: \"u_s2dTexture\"");
    }
    else
    {
        glUniform1i(iLocSampler, 0);  // Bind to texture unit 0
    }

    // Set clear color (dark blue background)
    glClearColor(0.0f, 0.0f, 0.2f, 1.0);

    return true;
}

/**
 * @brief Graphics rendering function for mipmap texture demonstration
 * 
 * This function renders a textured quad that demonstrates the effects of texture mipmapping:
 * - Binds the mipmap texture to show filtering effects at different distances
 * - Renders geometry with texture coordinates to showcase mipmap level selection
 * - Displays text overlay with sample information
 * 
 * The rendered quad will show different mipmap levels automatically based on the
 * texture's distance from the camera, demonstrating how OpenGL ES selects
 * appropriate mipmap levels for optimal visual quality and performance.
 */
void GraphicsUpdate(void)
{
    // Clear color and depth buffers for new frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);

    // Setup vertex position attributes
    glEnableVertexAttribArray(iLocPosition);
    glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices);

    // Setup texture coordinate attributes if available
    if(iLocTexCoord != -1)
    {
        glVertexAttribPointer(iLocTexCoord, 2, GL_FLOAT, GL_FALSE, 0, textureCoordinates);
        glEnableVertexAttribArray(iLocTexCoord);
    }

    // Bind the mipmap texture for rendering
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Render the textured quad (triangle strip with 4 vertices)
    glDrawElements(GL_TRIANGLE_STRIP, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
    
    // Render text overlay
	text->draw();

}

/**
 * @brief Main application entry point for texture mipmap demonstration
 * 
 * @return Exit status code (0 for success)
 * 
 * This application demonstrates texture mipmapping techniques in OpenGL ES 2.0:
 * - ETC1 compressed texture format usage
 * - Multiple mipmap generation and loading strategies
 * - Texture filtering effects comparison
 * 
 * The sample showcases three different approaches to mipmapping:
 * 1. Pre-generated mipmap levels loaded from separate files
 * 2. Auto-generated mipmaps using glGenerateMipmap()
 * 3. Disabled mipmapping for visual comparison
 * 
 * Mipmapping improves rendering performance and visual quality by providing
 * multiple resolution levels of the same texture, allowing the GPU to select
 * the most appropriate level based on the rendered object's distance.
 */
int main(void)
{

    // Initialize windowing system
    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    
    // Setup EGL context for OpenGL ES 2.0
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);
    
    // Initialize graphics resources and mipmap textures
    prepareGraphics(WINDOW_W, WINDOW_H);

    bool end = false;
    
    // Main rendering loop
    while(!end)
    {
        // Check for window events (close, minimize, etc.)
        if(podium->checkWindow() != XPodium::WINDOW_IDLE)
        {
            end = true;
        }
        
        // Render the mipmap demonstration scene
        GraphicsUpdate();

        // Present the rendered frame to display
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }
    
    // Clean up allocated resources
	delete text;
    glDeleteTextures(1, &textureID);
    CoreEGL::terminateEGL();
    podium->destroyWindow();

    return 0;
}
