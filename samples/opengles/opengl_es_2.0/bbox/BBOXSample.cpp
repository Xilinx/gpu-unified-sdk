/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include "FontEngine.h"
#include "XPodium.h"
#include "BBOXSample.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"

using std::string;
using std::vector;
using namespace SDKAMD;

struct BBox {
    string label;
    float x, y, w, h;
    float color[4]; // RGBA
};

vector<BBox> bboxes;
vector<float> vertices;
vector<float> colors;
FontEngine* text;

bool loadBBoxes(const string& filename) {
    string path = resourceDirectory + filename;
    std::ifstream file(path);
    if (!file.is_open()) {
        printf("Failed to open %s\n", path.c_str());
        return false;
    }

    string label;
    float x, y, w, h;
    bboxes.clear();
    vertices.clear();
    colors.clear();

    while (file >> label >> x >> y >> w >> h) {
        BBox bbox;
        bbox.label = label;
        bbox.x = x;
        bbox.y = y;
        bbox.w = w;
        bbox.h = h;

        // Assign color based on label
        if (label == "car") {
            bbox.color[0] = 1.0f; bbox.color[1] = 0.0f; bbox.color[2] = 0.0f; bbox.color[3] = 1.0f; // Red
        } else if (label == "truck") {
            bbox.color[0] = 0.0f; bbox.color[1] = 0.0f; bbox.color[2] = 1.0f; bbox.color[3] = 1.0f; // Blue
        } else if (label == "pedestrian") {
            bbox.color[0] = 0.0f; bbox.color[1] = 1.0f; bbox.color[2] = 0.0f; bbox.color[3] = 1.0f; // Green
        } else {
            bbox.color[0] = 1.0f; bbox.color[1] = 1.0f; bbox.color[2] = 1.0f; bbox.color[3] = 1.0f; // White
        }

        bboxes.push_back(bbox);

        // Define vertices for a rectangle (top-left, top-right, bottom-right, bottom-left)
        float left = x;
        float right = x + w;
        float top = y;
        float bottom = y + h;
        vertices.insert(vertices.end(), {
            left, top, 0.0f,
            right, top, 0.0f,
            right, bottom, 0.0f,
            left, bottom, 0.0f
        });

        // Repeat color for each vertex
        for (int i = 0; i < 4; ++i) {
            colors.insert(colors.end(), {bbox.color[0], bbox.color[1], bbox.color[2], bbox.color[3]});
        }
    }

    file.close();
    return true;
}

bool prepareGraphics(int width, int height) {
    printf("prepareGraphics(%d, %d)\n", width, height);
    string vertexShaderPath = resourceDirectory + vertexShaderFilename;
    string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

    GLuint vertexShaderID = 0;
    GLuint fragmentShaderID = 0;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize FontEngine
    text = new FontEngine(resourceDirectory.c_str(), width, height);

    // Load bounding box data
    if (!loadBBoxes("bbox_data.txt")) {
        printf("Failed to load bounding box data\n");
        return false;
    }

    // Add labels to FontEngine with adjusted position to match pedestrian placement
    for (const auto& bbox : bboxes) {
        text->addString(bbox.x, bbox.y + 5, bbox.label.c_str(), 
                        (int)(bbox.color[0] * 255), 
                        (int)(bbox.color[1] * 255), 
                        (int)(bbox.color[2] * 255), 255);
    }

    Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    printf("vertexShaderID = %d\n", vertexShaderID);
    Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);
    printf("fragmentShaderID = %d\n", fragmentShaderID);

    programID = glCreateProgram();
    if (programID == 0) {
        printf("Could not create program.\n");
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

void GraphicsUpdate(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);

    // Set up orthographic projection
    XMatrixAPI ortho = XMatrixAPI::XMatrixAPIOrthographic(0.0f, SCENE_WIDTH, SCENE_HEIGHT, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, ortho.getAsArray());

    // Render each bounding box
    size_t vertexOffset = 0;
    for (size_t i = 0; i < bboxes.size(); ++i) {
        glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices.data() + vertexOffset);
        glEnableVertexAttribArray(iLocPosition);

        if (iLocFillColor != -1) {
            glVertexAttribPointer(iLocFillColor, 4, GL_FLOAT, GL_FALSE, 0, colors.data() + vertexOffset * 4 / 3);
            glEnableVertexAttribArray(iLocFillColor);
        }

        glDrawArrays(GL_LINE_LOOP, 0, 4); // Draw rectangle as wireframe
        vertexOffset += 12; // 4 vertices * 3 floats
    }

    // Draw labels
    text->draw();
}

int main(int argc, char* argv[]) {
    XPodium* podium = XPodium::getHandler();
    // Size the window/surface and viewport to WINDOW_W x WINDOW_H
    // (GLES_WINDOW_W/H, default 1920x1080), matching the other samples. The
    // scene itself is laid out in a fixed SCENE_WIDTH x SCENE_HEIGHT design
    // space (see the ortho projection in GraphicsUpdate), which scales to fill
    // the window.
    podium->prepareWindow(WINDOW_W, WINDOW_H);

    if (argc >= 2) {
        int samples = atoi(argv[1]);
        CoreEGL::setEGLSamples(samples);
    }

    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);

    int attributeValue = -1;
    eglGetConfigAttrib(CoreEGL::display, CoreEGL::config, EGL_SAMPLES, &attributeValue);
    printf("EGL_SAMPLES = %d\n", attributeValue);

    if (!prepareGraphics(WINDOW_W, WINDOW_H)) {
        printf("Failed to initialize graphics\n");
        return 1;
    }

    bool loop = false;
    while (!loop) {
        if (podium->checkWindow() != XPodium::WINDOW_IDLE) {
            loop = true;
        }
        GraphicsUpdate();
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }

    delete text;
    CoreEGL::terminateEGL();
    podium->destroyWindow();
    delete podium;

    return 0;
}
