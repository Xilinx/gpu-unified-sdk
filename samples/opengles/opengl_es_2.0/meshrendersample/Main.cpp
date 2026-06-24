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

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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
GLint iLocTexCoord = -1;
GLint iLocMVP = -1;
GLint iLocNormalMatrix = -1;
GLint iLocLightSourcePosition = -1;
GLint iLocFillLightPosition = -1;
GLint iLocMaterialDiffuse = -1;
GLint iLocDiffuseTexture = -1;
GLuint modelTextureID = 0;
bool modelHasTexture_ = false;

GLuint VBO, EBO;
std::string fontResourceDirectory = "FontShaders/";
Mesh mesh_;
LibMatrix::mat4 perspective_;
LibMatrix::vec3 centerVec_;
float radius_ = 1.0f;
// Half-extents of the model's bounding box (set in loadModel). Used to frame
// the model tightly while keeping it fully on-screen as it spins about Y.
LibMatrix::vec3 halfExtent_ = LibMatrix::vec3(1.0f, 1.0f, 1.0f);
// The radius the model sweeps horizontally when rotated about the Y axis.
float spinRadius_ = 1.0f;
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

// Decode a PNG/JPG with stb_image and upload it as an RGBA GL texture.
// Returns true and sets outTextureID on success.
bool loadImageTexture(const std::string &path, GLuint &outTextureID) {
    int width = 0, height = 0, channels = 0;
    // Flip vertically: OBJ/UV origin is bottom-left, GL textures are too, but
    // most exported PNGs are top-left, so flipping gives correct orientation.
    stbi_set_flip_vertically_on_load(1);
    unsigned char *pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!pixels) {
        printf("loadImageTexture: failed to load '%s': %s\n", path.c_str(), stbi_failure_reason());
        return false;
    }

    GL_CHECK(glGenTextures(1, &outTextureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, outTextureID));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    stbi_image_free(pixels);
    printf("loadImageTexture: loaded '%s' (%dx%d)\n", path.c_str(), width, height);
    return true;
}

void createFullScreenQuad() {
    GLfloat vertices[] = {// Positions      // Texture Coords
                          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                          -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f};

    // 16-bit indices: OpenGL ES 2.0 only guarantees GL_UNSIGNED_BYTE /
    // GL_UNSIGNED_SHORT for glDrawElements. 32-bit (GL_UNSIGNED_INT) needs the
    // GL_OES_element_index_uint extension, which the Mali 400 (zynqmp) lacks -
    // using it there raises GL_INVALID_ENUM (0x0500) every frame.
    GLushort indices[] = {0, 1, 2, 1, 3, 2};

    GL_CHECK(glGenBuffers(1, &VBO));
    GL_CHECK(glGenBuffers(1, &EBO));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
}

// Vertical field of view (degrees) used for the perspective projection.
static const float kFovYDegrees = 45.0f;
// Extra breathing room around the model so it does not touch the screen edges.
static const float kFramingMargin = 1.05f;

// Compute how far back to place the camera so the model is framed tightly but
// stays fully visible while it spins about the Y axis. Accounts for both the
// vertical extent (fixed) and the horizontal sweep radius (X/Z), as well as the
// viewport aspect ratio (the horizontal FOV is wider than the vertical one).
float computeCameraDistance(float aspectRatio) {
    const float fovY = kFovYDegrees * static_cast<float>(M_PI) / 180.0f;
    const float tanHalfFovY = std::tan(fovY * 0.5f);
    const float tanHalfFovX = tanHalfFovY * aspectRatio;

    // Distance required so the vertical half-extent fits the vertical FOV.
    float distForHeight = halfExtent_.y() / tanHalfFovY;
    // Distance required so the horizontal sweep fits the horizontal FOV.
    float distForWidth = spinRadius_ / tanHalfFovX;

    // We must also clear the part of the model nearest the camera: when a point
    // at horizontal radius spinRadius_ rotates toward the viewer it sits
    // spinRadius_ closer than the center, so add it to avoid the model poking
    // through the near plane / camera.
    float dist = std::max(distForHeight, distForWidth) + spinRadius_;
    return dist * kFramingMargin;
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
    attribs.push_back(std::make_pair(Model::AttribTypeTexcoord, 2));

    model.convert_to_mesh(mesh_, attribs);

    printf("loadModel convertmesh sucess\n");

    centerVec_ = (model.maxVec() + model.minVec()) * 0.5f;
    LibMatrix::vec3 diffVec = model.maxVec() - model.minVec();
    radius_ = diffVec.length() * 0.5f;
    halfExtent_ = diffVec * 0.5f;

    // As the model spins about the Y axis, any point sweeps a circle of radius
    // sqrt(x^2 + z^2) about the center. The largest such radius (using the
    // box's X/Z half-extents) is the worst-case horizontal extent we must keep
    // on screen throughout the rotation.
    spinRadius_ = std::sqrt(halfExtent_.x() * halfExtent_.x() +
                            halfExtent_.z() * halfExtent_.z());

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
    GL_CHECK(iLocTexCoord = glGetAttribLocation(programID, "texcoord"));
    GL_CHECK(iLocMVP = glGetUniformLocation(programID, "ModelViewProjectionMatrix"));
    GL_CHECK(iLocNormalMatrix = glGetUniformLocation(programID, "NormalMatrix"));
    GL_CHECK(iLocLightSourcePosition = glGetUniformLocation(programID, "LightSourcePosition"));
    GL_CHECK(iLocFillLightPosition = glGetUniformLocation(programID, "FillLightPosition"));
    GL_CHECK(iLocMaterialDiffuse = glGetUniformLocation(programID, "MaterialDiffuse"));
    GL_CHECK(iLocDiffuseTexture = glGetUniformLocation(programID, "DiffuseTexture"));

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

    // Bundled default model is the CC0 "car" (models/car.obj). Drop any other
    // supported .obj/.3ds file into the models/ directory to render it instead.
    if (!loadModel("car")) {
        return false;
    }

    // Attribute locations must match the order passed to convert_to_mesh:
    // position, normal, texcoord.
    std::vector<GLint> attribLocations = {iLocPosition, iLocNormal, iLocTexCoord};
    mesh_.set_attrib_locations(attribLocations);

    // Load the model's diffuse texture, if one is bundled alongside it.
    modelHasTexture_ = loadImageTexture("models/car_texture.png", modelTextureID);
    if (!modelHasTexture_) {
        printf("prepareGraphics: no model texture loaded; model will render untextured.\n");
    }

    for (const auto &object : model.getObjects()) {
        // Default material properties for the model.
        LibMatrix::vec3 defaultColor = LibMatrix::vec3(0.8f, 0.8f, 0.8f);
        GL_CHECK(glUniform3fv(iLocMaterialDiffuse, 1, &defaultColor[0]));
    }

    if (useVbo_) {
        mesh_.build_vbo();
    } else {
        mesh_.build_array();
    }

    float aspectRatio = float(width) / float(height);
    LibMatrix::Stack4 stack;
    // Derive the near/far planes from the model's size so models of any scale
    // (e.g. a unit-sized OBJ or one exported in millimetres) stay inside the
    // view frustum. Use the same camera distance the draw loop uses so the far
    // plane always encloses the model.
    float camDist = computeCameraDistance(aspectRatio);
    float nearPlane = std::max(0.01f, radius_ * 0.01f);
    float farPlane = camDist + radius_ * 2.0f;
    stack.perspective(kFovYDegrees, aspectRatio, nearPlane, farPlane);
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
    GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0));
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
    GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0));
    GL_CHECK(glEnable(GL_DEPTH_TEST));

    renderCheckerboard();

    // Render the 3D model with depth testing enabled
    GL_CHECK(glUseProgram(programID));

    // Bind the model's diffuse texture to texture unit 0.
    if (modelHasTexture_) {
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, modelTextureID));
        if (iLocDiffuseTexture != -1) {
            GL_CHECK(glUniform1i(iLocDiffuseTexture, 0));
        }
    }

    // Frame the model tightly while keeping it fully visible as it spins about
    // the Y axis. Uses the same helper (and therefore the same distance) that
    // sized the projection's near/far planes in prepareGraphics.
    float aspectRatio = float(WINDOW_W) / float(WINDOW_H);
    float cameraDistance = computeCameraDistance(aspectRatio);

    // Lighting is computed in model space, so position the lights and eye
    // relative to the model's center and scale by its radius. This keeps the
    // look consistent regardless of the model's units/size. Lighting is in
    // object space, so the lights are anchored to the car body and the lit
    // sides rotate with it (a clean, stable turntable look).
    const LibMatrix::vec3 &c = centerVec_;
    float r = radius_;

    // Key light: high, to the front-left of the model.
    LibMatrix::vec3 keyLightPos =
        c + LibMatrix::vec3(-1.0f * r, 1.6f * r, 1.4f * r);
    GL_CHECK(glUniform3fv(iLocLightSourcePosition, 1, &keyLightPos[0]));

    // Fill light: lower, to the opposite side, to soften shadows.
    LibMatrix::vec3 fillLightPos =
        c + LibMatrix::vec3(1.5f * r, 0.4f * r, -0.8f * r);
    if (iLocFillLightPosition != -1) {
        GL_CHECK(glUniform3fv(iLocFillLightPosition, 1, &fillLightPos[0]));
    }

    // Glossy, slightly warm paint highlight.
    LibMatrix::vec3 specularColor = LibMatrix::vec3(0.9f, 0.85f, 0.8f);
    GL_CHECK(glUniform3fv(glGetUniformLocation(programID, "MaterialSpecular"), 1, &specularColor[0]));
    float shininess = 80.0f;
    GL_CHECK(glUniform1f(glGetUniformLocation(programID, "Shininess"), shininess));

    // Eye is where the camera actually sits: the model is pushed back by
    // cameraDistance along -Z, so in model space the eye is in front of the
    // model center along +Z.
    LibMatrix::vec3 eyePosition = c + LibMatrix::vec3(0.0f, 0.0f, cameraDistance);
    GL_CHECK(glUniform3fv(glGetUniformLocation(programID, "EyePosition"), 1, &eyePosition[0]));

    LibMatrix::Stack4 model_view;

    // Compose transforms so the model spins about its own center: first move
    // the model's center to the origin, rotate there, then push it away from
    // the camera. Stack4 post-multiplies, so issue them in reverse order.
    rotation_ += rotationSpeed_ * 0.04f;
    model_view.translate(0.0f, 0.0f, -cameraDistance);
    model_view.rotate(rotation_, 0.0f, 1.0f, 0.0f);
    model_view.translate(-centerVec_.x(), -centerVec_.y(), -centerVec_.z());
    LibMatrix::mat4 model_view_proj = perspective_ * model_view.getCurrent();

    GL_CHECK(glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, &model_view_proj[0][0]));
    // Lighting is done in object space (see light-basic.vert), so no separate
    // normal matrix is needed: the spin is applied only to the clip-space
    // position via ModelViewProjectionMatrix.

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
 * - Complex 3D model loading and parsing (.obj / .3ds formats)
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
