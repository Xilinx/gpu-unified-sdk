#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "3DModel.h"
#include "FontEngine.h"
#include "XEGLIntf.h"
#include "XGLSLCompile.h"
#include "XPodium.h"
#include "mat.h"
#include "mesh.h"
#include "stack.h"
#include <algorithm>
#include <set>
#include <unistd.h>

#include "XTime.h"
#include <iomanip>
#include <sstream>

#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

SDKXilinx::Timer fpsTimer;
std::string fpsString = "FPS: 0.00";
float lastFpsUpdate = 0.0f;

using std::string;
using namespace SDKXilinx;
using namespace std;

GLuint programID;
GLuint checkerProgramID;
GLuint checkerTextureID;
GLuint fullScreenQuadVAO;
GLint iLocPosition = -1;
GLint iLocNormal = -1;
GLint iLocMVP = -1;
GLint iLocNormalMatrix = -1;
GLint iLocLightSourcePosition = -1;
GLint iLocMaterialDiffuse = -1;

GLuint VBO, EBO;
std::string fontResourceDirectory = "FontShaders/";
Mesh mesh_;
LibMatrix::mat4 perspective_;
LibMatrix::vec3 centerVec_;
float radius_ = 1.0f;
bool useVbo_ = true;
bool orientModel_ = false;
float rotation_ = 0.0f;
float rotationSpeed_ = 36.0f;
LibMatrix::vec3 orientationVec_;
float orientationAngle_ = 0.0f;
Model model1, model;
FontEngine *text;

void checkOpenGLError(const char *stmt, const char *fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
    }
}

#define GL_CHECK(stmt)                                                                                                 \
    do {                                                                                                               \
        stmt;                                                                                                          \
        checkOpenGLError(#stmt, __FILE__, __LINE__);                                                                   \
    } while (0)

bool loadCheckerboardTexture() {
    // Create checkerboard pattern data
    const int checkerSize = 8;
    const int textureWidth = 64;
    const int textureHeight = 64;
    GLubyte checkerImage[textureHeight][textureWidth][4];

    for (int y = 0; y < textureHeight; y++) {
        for (int x = 0; x < textureWidth; x++) {
            GLubyte c = (((x & checkerSize) == 0) ^ ((y & checkerSize) == 0)) * 255;
            checkerImage[y][x][0] = c;
            checkerImage[y][x][1] = c;
            checkerImage[y][x][2] = c;
            checkerImage[y][x][3] = 255;
        }
    }

    // Generate and bind texture
    GL_CHECK(glGenTextures(1, &checkerTextureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, checkerTextureID));

    // Set texture parameters
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                          checkerImage));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    return true;
}

void createFullScreenQuad() {
    GLfloat vertices[] = {// Positions      // Texture Coords
                          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                          -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f};

    GLuint indices[] = {0, 1, 2, 1, 3, 2};

    GL_CHECK(glGenBuffers(1, &VBO));
    GL_CHECK(glGenBuffers(1, &EBO));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
}

bool loadModel(const std::string &modelName) {
    if (!model.load(modelName)) {
        return false;
    }

    printf("loadModel sucess\n");
    if (model.needNormals()) {
        model.calculate_normals();
    }

    printf("loadModel Normal sucess\n");

    std::vector<std::pair<Model::AttribType, int>> attribs;
    attribs.push_back(std::make_pair(Model::AttribTypePosition, 3));
    attribs.push_back(std::make_pair(Model::AttribTypeNormal, 3));

    model.convert_to_mesh(mesh_, attribs);

    printf("loadModel convertmesh sucess\n");

    centerVec_ = (model.maxVec() + model.minVec()) * 0.5f;
    LibMatrix::vec3 diffVec = model.maxVec() - model.minVec();
    radius_ = diffVec.length() * 0.5f;

    return true;
}

bool prepareGraphics(int width, int height) {
    string vertexShaderPath = "Supportingfiles/light-basic.vert";
    string fragmentShaderPath = "Supportingfiles/light-basic.frag";

    GLuint vertexShaderID, fragmentShaderID;
    GL_CHECK(Shader::processShader(&vertexShaderID, vertexShaderPath.c_str(), GL_VERTEX_SHADER));
    GL_CHECK(Shader::processShader(&fragmentShaderID, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER));

    GL_CHECK(programID = glCreateProgram());
    if (programID == 0) {
        printf("prepareGraphics: Could not create program.\n");
        return false;
    }

    GL_CHECK(glAttachShader(programID, vertexShaderID));
    GL_CHECK(glAttachShader(programID, fragmentShaderID));
    GL_CHECK(glLinkProgram(programID));

    GLint linkStatus;
    GL_CHECK(glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus));
    if (linkStatus != GL_TRUE) {
        char infoLog[512];
        GL_CHECK(glGetProgramInfoLog(programID, 512, NULL, infoLog));
        return false;
    }

    GL_CHECK(glUseProgram(programID));

    GL_CHECK(iLocPosition = glGetAttribLocation(programID, "position"));
    GL_CHECK(iLocNormal = glGetAttribLocation(programID, "normal"));
    GL_CHECK(iLocMVP = glGetUniformLocation(programID, "ModelViewProjectionMatrix"));
    GL_CHECK(iLocNormalMatrix = glGetUniformLocation(programID, "NormalMatrix"));
    GL_CHECK(iLocLightSourcePosition = glGetUniformLocation(programID, "LightSourcePosition"));
    GL_CHECK(iLocMaterialDiffuse = glGetUniformLocation(programID, "MaterialDiffuse"));

    GL_CHECK(glViewport(0, 0, width, height));
    GL_CHECK(glClearColor(0.8f, 0.8f, 0.0f, 1.0f));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glCullFace(GL_BACK));
    text = new FontEngine(fontResourceDirectory.c_str(), width, height);
    text->addString(0, 0, "RenderBlitzSDK: Mesh Rendering Sample", 255, 0, 0, 255);

    // Load shaders for the checkerboard background
    string checkerVertexShaderPath = "Supportingfiles/checker.vert";
    string checkerFragmentShaderPath = "Supportingfiles/checker.frag";

    GL_CHECK(Shader::processShader(&vertexShaderID, checkerVertexShaderPath.c_str(), GL_VERTEX_SHADER));
    GL_CHECK(Shader::processShader(&fragmentShaderID, checkerFragmentShaderPath.c_str(), GL_FRAGMENT_SHADER));

    GL_CHECK(checkerProgramID = glCreateProgram());
    if (checkerProgramID == 0) {
        printf("prepareGraphics: Could not create checker program.\n");
        return false;
    }

    GL_CHECK(glAttachShader(checkerProgramID, vertexShaderID));
    GL_CHECK(glAttachShader(checkerProgramID, fragmentShaderID));
    GL_CHECK(glLinkProgram(checkerProgramID));

    GL_CHECK(glGetProgramiv(checkerProgramID, GL_LINK_STATUS, &linkStatus));
    if (linkStatus != GL_TRUE) {
        char infoLog[512];
        GL_CHECK(glGetProgramInfoLog(checkerProgramID, 512, NULL, infoLog));
        return false;
    }

    GL_CHECK(glUseProgram(checkerProgramID));
    createFullScreenQuad();
    loadCheckerboardTexture();

    if (!loadModel("audi")) {
        return false;
    }

    std::vector<GLint> attribLocations = {iLocPosition, iLocNormal};
    mesh_.set_attrib_locations(attribLocations);

    for (const auto &object : model.getObjects()) {
        if (object.name.find("wheelsA.002_RubberRough.002") != std::string::npos) {
            // Set material properties for chrome parts
            printf("Applying chrome color to: %s\n", object.name.c_str());
            LibMatrix::vec3 chromeColor = LibMatrix::vec3(1.0f, 1.0f, 1.0f); // Silver-like color
            GL_CHECK(glUniform3fv(iLocMaterialDiffuse, 1, &chromeColor[0]));
        } else {
            // Default material properties
            LibMatrix::vec3 defaultColor = LibMatrix::vec3(0.8f, 0.8f, 0.8f);
            GL_CHECK(glUniform3fv(iLocMaterialDiffuse, 1, &defaultColor[0]));
        }
    }

    if (useVbo_) {
        mesh_.build_vbo();
    } else {
        mesh_.build_array();
    }

    float aspectRatio = float(width) / float(height);
    LibMatrix::Stack4 stack;
    stack.perspective(45.0f, aspectRatio, 0.01f, 100.0f);
    perspective_ = stack.getCurrent();

    return true;
}

void renderCheckerboard() {
    GL_CHECK(glUseProgram(checkerProgramID));

    GLint texLoc = glGetUniformLocation(checkerProgramID, "checkerTexture");
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, checkerTextureID));
    GL_CHECK(glUniform1i(texLoc, 0));

    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
}

void GraphicsUpdate(void) {
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // Update FPS display
    float currentFPS = fpsTimer.getFPS();
    float currentTime = fpsTimer.getTime();

    std::ostringstream fpsStream;
    fpsStream << "FPS: " << std::fixed << std::setprecision(1) << currentFPS;
    fpsString = fpsStream.str();

    // Print to terminal every second
    if (currentTime - lastFpsUpdate >= 1.0f) {
        printf("FPS: %.1f\n", currentFPS);
        lastFpsUpdate = currentTime;
    }

    // Render the checkerboard background
    GL_CHECK(glUseProgram(checkerProgramID));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));

    GLint posAttrib = glGetAttribLocation(checkerProgramID, "position");
    GLint texAttrib = glGetAttribLocation(checkerProgramID, "texcoord");

    GL_CHECK(glEnableVertexAttribArray(posAttrib));
    GL_CHECK(glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0));

    GL_CHECK(glEnableVertexAttribArray(texAttrib));
    GL_CHECK(
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat))));

    GL_CHECK(glDisable(GL_DEPTH_TEST));
    GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
    GL_CHECK(glEnable(GL_DEPTH_TEST));

    renderCheckerboard();

    // Render the 3D model with depth testing enabled
    GL_CHECK(glUseProgram(programID));

    LibMatrix::vec3 lightPos = LibMatrix::vec3(50.0f, 50.0f, 5.0f);
    GL_CHECK(glUniform3fv(iLocLightSourcePosition, 1, &lightPos[0]));

    LibMatrix::vec3 specularColor = LibMatrix::vec3(1.0f, 0.0f, 0.0f);
    GL_CHECK(glUniform3fv(glGetUniformLocation(programID, "MaterialSpecular"), 1, &specularColor[0]));
    float shininess = 35.0f;
    GL_CHECK(glUniform1f(glGetUniformLocation(programID, "Shininess"), shininess));
    LibMatrix::vec3 eyePosition = LibMatrix::vec3(0.0f, 0.0f, 5.0f); // Simple camera position
    GL_CHECK(glUniform3fv(glGetUniformLocation(programID, "EyePosition"), 1, &eyePosition[0]));

    LibMatrix::Stack4 model_view;
    model_view.translate(-centerVec_.x(), -centerVec_.y(),
                         -centerVec_.z() - 1.5f); // Centering object in front of camera

    rotation_ += rotationSpeed_ * 0.04f;
    model_view.rotate(rotation_, 0.0f, 1.0f, 0.0f);
    LibMatrix::mat4 model_view_proj = perspective_ * model_view.getCurrent();

    GL_CHECK(glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, &model_view_proj[0][0]));
    LibMatrix::mat4 normal_matrix = model_view.getCurrent();
    normal_matrix.inverse().transpose();
    GL_CHECK(glUniformMatrix4fv(iLocNormalMatrix, 1, GL_FALSE, &normal_matrix[0][0]));

    if (useVbo_) {
        for (const auto &object : model.getObjects()) {
            // printf("Rendering object: %s with %zu vertices\n", object.name.c_str(), object.vertices.size());

            LibMatrix::vec3 defaultColor = LibMatrix::vec3(1.0f, 0.0f, 0.0f);
            GL_CHECK(glUniform3fv(iLocMaterialDiffuse, 1, &defaultColor[0]));

            mesh_.render_vbo(programID);
            break; // Render just the first object for debugging
        }
    } else {
        mesh_.render_array();
    }

    // eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    text->addString(0, 0, fpsString.c_str(), 255, 0, 0, 255); // #NS - does not work yet
    text->draw();
}

/**
 * @brief Main application entry point for 3D mesh rendering demonstration
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Exit status code (0 for success, 1 for failure)
 * 
 * This application demonstrates advanced 3D mesh rendering capabilities:
 * - Complex 3D model loading and parsing (.obj or proprietary formats)
 * - Multiple mesh rendering with different materials and textures
 * - Camera controls and view matrix manipulation
 * - Real-time lighting calculations and material properties
 * - Performance monitoring with FPS tracking
 * 
 * Features showcased:
 * - Vertex buffer management for complex geometry
 * - Multi-pass rendering for different objects
 * - Dynamic camera positioning and orientation
 * - Efficient mesh data structures and rendering
 * - Real-time 3D scene management
 * 
 * This sample serves as a foundation for 3D applications requiring
 * complex scene rendering with multiple objects and materials.
 */
int main(int argc, char *argv[]) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }

    Model::find_models();

    XPodium *podium = XPodium::getHandler();
    podium->prepareWindow(WINDOW_W, WINDOW_H);
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);

    if (!prepareGraphics(WINDOW_W, WINDOW_H)) {
        printf("Failed to prepare graphics.\n");
        return -1;
    }

    bool loop = false;

    fpsTimer.reset();

    while (!loop) {
        if (podium->checkWindow() != XPodium::WINDOW_IDLE) {
            loop = true;
        }

        GraphicsUpdate();
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }
    printf("Final FPS: %.1f\n", fpsTimer.getFPS());
    delete text;
    CoreEGL::terminateEGL();
    podium->destroyWindow();

    return 0;
}
