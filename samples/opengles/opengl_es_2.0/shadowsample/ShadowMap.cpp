/*
 *
 * Copyright (c) _2024, Advanced Micro Devices, Inc.
 * All rights reserved.
 * Developed by:
 *           AMD Software Development
 *           Advanced Micro Devices, Inc.
 *           www.amd.com

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to
 * deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimers in
 *   the documentation and/or other materials provided with the distribution.
 * - Neither the names of <Name of Development Group, Name of Institution>,
 *   nor the names of its contributors may be used to endorse or promote
 *   products derived from this Software without specific prior written
 *   permission.
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 */

#include "ShadowMap.h"

#include "FontEngine.h"
#include "MAnimation.h"
#include "MDRRenderTarget.h"
#include "MDRRendererTexture.h"
#include "MGeometryComplex.h"
#include "MGeometryRectangle.h"
#include "MGeometryTorus.h"
#include "MPathsManager.h"
#include "MRendererProgram.h"
#include <fstream>
#include <iostream>
#include <vector>

#include "3d.h"


#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

using std::string;
using namespace std;
using namespace SDKXilinx;
FontEngine *text;

std::string fontResourceDirectory = "FontShaders/";
std::string resourceDirectory = "Supportingfiles/";

#define VERTEX_SHADER_FILE "ShadowMap_shadowmap.vert"
#define FRAGMENT_SHADER_FILE "ShadowMap_shadowmap.frag"
#define VERTEX_SHADER_SHADOW_FILE "ShadowMap_shadow.vert"
#define FRAGMENT_SHADER_SHADOW_FILE "ShadowMap_shadow.frag"
#define VERTEX_SHADER_SOLID_FILE "ShadowMap_solid.vert"
#define FRAGMENT_SHADER_SOLID_FILE "ShadowMap_solid.frag"
#define VERTEX_SHADER_FLARE_FILE "ShadowMap_flare.vert"
#define FRAGMENT_SHADER_FLARE_FILE "ShadowMap_flare.frag"
#define FLARE_IMAGE_FILE "ShadowMap_Flare.tga"

std::string vertexShaderFilename = "ShadowMap_shadowmap.vert";
std::string fragmentShaderFilename = "ShadowMap_shadowmap.frag";
std::string vertexShaderShadowFilename = "ShadowMap_shadow.vert";
std::string fragmentShaderShadowFilename = "ShadowMap_shadow.frag";
std::string vertexShaderSolidFilename = "ShadowMap_solid.vert";
std::string fragmentShaderSolidFilename = "ShadowMap_solid.frag";
std::string vertexShaderFlareFilename = "ShadowMap_flare.vert";
std::string fragmentShaderFlareFilename = "ShadowMap_flare.frag";
std::string flareImageFilename = "ShadowMap_Flare.tga";

std::string vertexShaderPath = resourceDirectory + vertexShaderFilename;
std::string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;
std::string vertexShaderShadowPath = resourceDirectory + vertexShaderShadowFilename;
std::string fragmentShaderShadowPath = resourceDirectory + fragmentShaderShadowFilename;
std::string vertexShaderSolidPath = resourceDirectory + vertexShaderSolidFilename;
std::string fragmentShaderSolidPath = resourceDirectory + fragmentShaderSolidFilename;
std::string vertexShaderFlarePath = resourceDirectory + vertexShaderFlareFilename;
std::string fragmentShaderFlarePath = resourceDirectory + fragmentShaderFlareFilename;
std::string flareImagePath = resourceDirectory + flareImageFilename;

struct TGAImage {
    unsigned int width;
    unsigned int height;
    std::vector<unsigned char> data;
};

bool loadTGA(const std::string &filename, TGAImage &image) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open TGA file: " << filename << std::endl;
        return false;
    }

    // Read TGA header
    unsigned char header[18];
    file.read(reinterpret_cast<char *>(header), 18);

    // Extract width, height, and pixel depth from header
    image.width = header[12] + (header[13] << 8);
    image.height = header[14] + (header[15] << 8);
    unsigned char pixelDepth = header[16];

    if (pixelDepth != 24 && pixelDepth != 32) {
        std::cerr << "Unsupported TGA pixel depth: " << (int)pixelDepth << std::endl;
        return false;
    }

    unsigned int bytesPerPixel = pixelDepth / 8;
    unsigned int imageSize = image.width * image.height * bytesPerPixel;
    image.data.resize(imageSize);

    file.read(reinterpret_cast<char *>(image.data.data()), imageSize);
    file.close();

    // TGA images are stored in BGR(A) format, we need to convert them to RGB(A)
    for (unsigned int i = 0; i < imageSize; i += bytesPerPixel) {
        std::swap(image.data[i], image.data[i + 2]);
    }

    return true;
}

/* Declare variables. */
MRendererProgram *theProgramSolid = NULL;
MRendererProgram *theProgramShadow = NULL;
MRendererProgram *theProgramShadowMap = NULL;
MRendererProgram *theProgramFlare = NULL;
MAnimation1f theCamRotYAnim;
MAnimation1f theLightRotYAnim;
MTime theTime;

struct timeval sNow;

/* Animation variables. */
struct timeval sLastAnim1;
struct timeval sLastAnim2;

/* Scene transformation matrices. */
XMatrixAPI sMatLightM;
XMatrixAPI sMatLightV;
XMatrixAPI sMatLightProj;

XMatrixAPI sMatSceneV;
XMatrixAPI sMatSceneProj;
XMatrixAPI sMatCameraV;

XMatrixAPI sMatIdentity;

MGeometryTorus *theGeometryTorus = NULL;
MGeometryTorus *newGeometryTorus = NULL;
MGeometryComplex *theGeometryComplex = NULL;
MGeometryRectangle *theGeometryRectangle = NULL;

MVector4f theGlobalAmbient;

MDRRendererTexture *theTextureFlare = NULL;
MDRRenderTarget *theRT = NULL;

/* FPS variables. */
struct timeval sLastFPS;
unsigned int uiFrame = 0;
long int lElapsedFPS = 0L;

int g_iWindowW = -1;
int g_iWindowH = -1;
int g_iShadowW = -1;
int g_iShadowH = -1;

bool setupGraphics(int width, int height) {
    g_iWindowW = width;
    g_iWindowH = height;

    g_iShadowW = width;
    g_iShadowH = height;

    /* Allocate global variables */
    theProgramSolid = new MRendererProgram;
    theProgramShadow = new MRendererProgram;
    theProgramShadowMap = new MRendererProgram;
    theProgramFlare = new MRendererProgram;
    newGeometryTorus = new MGeometryTorus;
    theGeometryTorus = new MGeometryTorus;
    theGeometryComplex = new MGeometryComplex;
    theGeometryRectangle = new MGeometryRectangle;
    theTextureFlare = new MDRRendererTexture;
    theRT = new MDRRenderTarget(MDRRenderTarget::MODE_COPY_TEXTURE);

    theGlobalAmbient.set(0.2f, 0.2f, 0.2f, 1.0f);

    /* Define animations. */
    theCamRotYAnim.setMode(MAnimation1f::PLAY_MODE_REPEAT);
    theCamRotYAnim.setKey(0.0f, MTime(0.0f));
    theCamRotYAnim.setKey(-360, MTime(20.0f));

    theLightRotYAnim.setMode(MAnimation1f::PLAY_MODE_REPEAT);
    theLightRotYAnim.setKey(0.0f, MTime(0.0f));
    theLightRotYAnim.setKey(360, MTime(20.0f));

    /* Set sphere resolution (horizontal and vertical density of triangles). */
    newGeometryTorus->set(0.5f, 0.18f, 20, 20);
    theGeometryTorus->set(0.32f, 0.18f, 20, 10);

    theGeometryTorus->getPrimitive().transform(MVector3f(0.0f, 0.0f, 0.0f));
    theGeometryComplex->set(theGeometryTorus->getPrimitive());

    newGeometryTorus->getPrimitive().transform(MVector3f(1.3f, 0.0f, 0.0f));
    theGeometryComplex->append(newGeometryTorus->getPrimitive());
    newGeometryTorus->getPrimitive().transform(MVector3f(-2.6f, 0.0f, 0.0f));
    theGeometryComplex->append(newGeometryTorus->getPrimitive());
    newGeometryTorus->getPrimitive().transform(MVector3f(0.7f, 0.0f, -1.3f));
    theGeometryComplex->append(newGeometryTorus->getPrimitive());
    newGeometryTorus->getPrimitive().transform(MVector3f(0.6f, 0.0f, 1.3f));

    theGeometryRectangle->set(5.0f, 5.0f, 1, 1);

    /* Initialize matrices. */
    sMatSceneProj = XMatrixAPI::XMatrixAPIPerspective(tan(M_PI / 4.0f), (g_iWindowW / (float)g_iWindowH), 1.0f, 100.0f);
    sMatLightProj = XMatrixAPI::XMatrixAPIPerspective(tan(M_PI / 4.0f), (g_iShadowW / (float)g_iShadowH), 1.0f, 100.0f);
    /* Create scene modelview matrix, move geometry further away from camera. */
    {
        XMatrixAPI sMatSceneRot = XMatrixAPI::createRotationX(10);
        XMatrixAPI sMatSceneRoty = XMatrixAPI::createRotationY(20);
        sMatSceneV = XMatrixAPI::createTranslation(0.0f, 0.0f, -4.0f);
        sMatSceneV = sMatSceneV * sMatSceneRot * sMatSceneRoty;
    }
    sMatIdentity = XMatrixAPI::identityXMatrixAPI;

    /* Initialize OpenGL-ES. */
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glCullFace(GL_BACK));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthMask(true));
    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glEnable(GL_BLEND));

    /* Should do src * (src alpha) + dest * (1-src alpha). */
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    text = new FontEngine(fontResourceDirectory.c_str(), width, height);
    text->addString(0, 0, "RenderBlitzSDK: Shadow Mapping Sample", 255, 0, 0, 255);
    theRT->initialize(g_iShadowW, g_iShadowH);

    /* Load shaders. */
    theProgramFlare->initialize(vertexShaderFlarePath, fragmentShaderFlarePath);
    /* Bind attribute parameters. */
    theProgramFlare->bindAttrib(MRendererProgram::A_LOC_VERTEX, "a_v4Position");
    theProgramFlare->bindAttrib(MRendererProgram::A_LOC_TEXCOORD_0, "a_v3TexCoord");
    /* Bind uniform parameters. */
    theProgramFlare->bindUniform(MRendererProgram::U_LOC_MAT_MVP, "u_m4MVP");
    theProgramFlare->bindUniform(MRendererProgram::U_LOC_GENERIC_1, "u_v4FillColor");
    theProgramFlare->bindUniform(MRendererProgram::U_LOC_SAMPLER_0, "u_s2dShadowMap");
    /* Set default parameters. */
    theProgramFlare->setUniform(MRendererProgram::U_LOC_GENERIC_1, MVector4f(1.0f, 1.0f, 1.0f, 1.0f));
    theProgramFlare->setUniform(MRendererProgram::U_LOC_SAMPLER_0, 0);

    /* Load Shader for drawing objects into shadow map. */
    theProgramSolid->initialize(vertexShaderSolidPath, fragmentShaderSolidPath);
    /* Bind attribute parameters. */
    theProgramSolid->bindAttrib(MRendererProgram::A_LOC_VERTEX, "a_v4Position");
    /* Bind uniform parameters. */
    theProgramSolid->bindUniform(MRendererProgram::U_LOC_MAT_MVP, "u_m4MVP");
    theProgramSolid->bindUniform(MRendererProgram::U_LOC_GENERIC_1, "u_v4FillColor");
    /* Set default parameters. */
    theProgramSolid->setUniform(MRendererProgram::U_LOC_GENERIC_1, MVector4f(1.0f, 1.0f, 0.0f, 1.0f));

    /* Load Shader for drawing objects into shadow map. */
    theProgramShadow->initialize(vertexShaderShadowPath, fragmentShaderShadowPath);
    /* Bind attribute parameters. */
    theProgramShadow->bindAttrib(MRendererProgram::A_LOC_VERTEX, "a_v4Position");
    /* Bind uniform parameters. */
    theProgramShadow->bindUniform(MRendererProgram::U_LOC_MAT_MVP, "u_m4LightMVP");

    /* Load ShadowMap shader which draws object with shadows on them. */
    theProgramShadowMap->initialize(vertexShaderPath, fragmentShaderPath);
    /* Bind attribute parameters. */
    theProgramShadowMap->bindAttrib(MRendererProgram::A_LOC_VERTEX, "a_v4Position");
    theProgramShadowMap->bindAttrib(MRendererProgram::A_LOC_COLOR, "a_v4FillColor");
    theProgramShadowMap->bindAttrib(MRendererProgram::A_LOC_NORMAL, "a_v4Normal");
    /* Bind uniforms parameters. */
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_MAT_MVP, "u_m4MVP");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_MAT_M, "u_m4M");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_MAT_MV, "u_m4MV");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_MAT_MV_INV_TRANS, "u_m4InvTransMV");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_MAT_0, "u_m4LightV");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_MAT_1, "u_m4LightP");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_SAMPLER_0, "u_s2dShadowMap");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_GENERIC_1, "u_v3Specular");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_GENERIC_2, "u_v4LightPos");
    theProgramShadowMap->bindUniform(MRendererProgram::U_LOC_GENERIC_3, "u_v3GlobalAmbient");
    /* Bind u_s2dShadowMap onto first texture unit. */
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_SAMPLER_0, 0);

    /* Load textures */
    TGAImage image;
    cout << MPathsManager::getFullPathStatic(FLARE_IMAGE_FILE).getData() << endl;
    if (loadTGA(MPathsManager::getFullPathStatic(FLARE_IMAGE_FILE).getData(), image)) {
        theTextureFlare->create(image.width, image.height, image.data.data());
    } else {
        cerr << "Failed to load flare image." << endl;
        return false;
    }

    return true;
}

void terminateGraphics() {
    deleteAndNULL(theRT);
    deleteAndNULL(theTextureFlare);
    deleteAndNULL(theGeometryRectangle);
    deleteAndNULL(theGeometryComplex);
    deleteAndNULL(theGeometryTorus);
    deleteAndNULL(newGeometryTorus);
    deleteAndNULL(theProgramFlare);
    deleteAndNULL(theProgramShadowMap);
    deleteAndNULL(theProgramShadow);
    deleteAndNULL(theProgramSolid);
}

void renderGeometry(MGeometryBase &aGeometry, MRendererProgram &aProgram, XMatrixAPI &aM, XMatrixAPI &aV,
                    XMatrixAPI &aP) {
    XMatrixAPI mv = aV * aM;
    XMatrixAPI mvp = aP * mv;
    XMatrixAPI itmv = XMatrixAPI::XMatrixAPIInvert(&mv);
    XMatrixAPI::XMatrixAPITranspose(&itmv);

    aProgram.use();

    aProgram.setUniform(MRendererProgram::U_LOC_MAT_M, aM);
    aProgram.setUniform(MRendererProgram::U_LOC_MAT_MV, mv);
    aProgram.setUniform(MRendererProgram::U_LOC_MAT_MV_INV_TRANS, itmv);
    aProgram.setUniform(MRendererProgram::U_LOC_MAT_MVP, mvp);

    aGeometry.render(aProgram);
}

void renderFrame(void) {
    theTime.update();
    theCamRotYAnim.update(theTime);
    theLightRotYAnim.update(theTime);

    /* Create modelview matrix for light, looking at middle of far cube. */
    XMatrixAPI sMatTrans = XMatrixAPI::createTranslation(0.0, 0.0f, -4.0f);
    XMatrixAPI sMatRotX = XMatrixAPI::createRotationX(30.0f);
    XMatrixAPI sMatRotY = XMatrixAPI::createRotationY(theLightRotYAnim.getValue());

    sMatLightV = sMatTrans * sMatRotX;
    sMatLightV = sMatLightV * sMatRotY;
    sMatLightM = XMatrixAPI::XMatrixAPIInvert(&sMatLightV);

    sMatRotY = XMatrixAPI::createRotationY(theCamRotYAnim.getValue());
    sMatCameraV = sMatSceneV * sMatRotY;

    /* Set up modelview and projection matrices for Light's scene. */
    XMatrixAPI matModel = XMatrixAPI::identityXMatrixAPI;
    {
        XMatrixAPI mT = XMatrixAPI::createTranslation(0.0f, 1.0f, 1.2f);
        XMatrixAPI mR = XMatrixAPI::createRotationX(theCamRotYAnim.getValue() * 2.0f);
        matModel = mT * mR;
    }

    /* Set up modelview and projection matrices for Light's scene. */
    XMatrixAPI matLightInCamera = sMatCameraV * sMatLightM;
    CVec4f lightPos = {0.0f, 0.0f, 0.0f, 1.0f};
    lightPos = XMatrixAPI::vertexTransform(&lightPos, &matLightInCamera);

    /* Calculate ambient for current frame */
    {
        MVector3f ambientCoef(lightPos.x, lightPos.y, lightPos.z);
        ambientCoef.normalize();
        float cosAlpha = ambientCoef.dot(MVector3f(0.0, 0.0, -1.0)) * 5.0f - 3.9f;

        if (cosAlpha < 0.0f)
            cosAlpha = 0.0f;
        if (cosAlpha > 1.0f)
            cosAlpha = 1.0f;

        theGlobalAmbient.set(cosAlpha, cosAlpha, cosAlpha, 1.0f);
    }

    theProgramShadowMap->use();
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_GENERIC_2, lightPos);
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_MAT_0, sMatLightV);
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_MAT_1, sMatLightProj);
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_GENERIC_1, MVector4f(1.0f, 1.0f, 0.0f, 1.0f));
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_GENERIC_3, theGlobalAmbient);
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_SAMPLER_0, 0);

    /* Prepare shadow texture - render scene into texture from light point of view. */

    if (!theRT->bindRT()) {
        LOGI("Error: Framebuffer incomplete at %s:%i\n", __FILE__, __LINE__);
        exit(1);
    }

    /* Set clear screen color and clear screen. */
    GL_CHECK(glClearColor(0.8f, 0.8f, 0.0f, 1.0f));
    GL_CHECK(glClearDepthf(1.0f));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthMask(true));
    GL_CHECK(glDepthFunc(GL_LEQUAL));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    GL_CHECK(glCullFace(GL_BACK));

    renderGeometry(*theGeometryComplex, *theProgramShadow, sMatIdentity, sMatLightV, sMatLightProj);

    renderGeometry(*theGeometryTorus, *theProgramShadow, matModel, sMatLightV, sMatLightProj);

    theRT->unbindRT();

    /* Draw Scene with shadows into framebuffer and use created shadow texture. */

    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    theRT->bindTexture();

    /* Render main scene. */
    GL_CHECK(glViewport(0, 0, g_iWindowW, g_iWindowH));

    GL_CHECK(glClearColor(theGlobalAmbient[0], theGlobalAmbient[1], theGlobalAmbient[2], theGlobalAmbient[3]));
    GL_CHECK(glClearDepthf(1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GL_CHECK(glCullFace(GL_BACK));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthMask(true));
    GL_CHECK(glDepthFunc(GL_LEQUAL));

    renderGeometry(*theGeometryComplex, *theProgramShadowMap, sMatIdentity, sMatCameraV, sMatSceneProj);

    renderGeometry(*theGeometryTorus, *theProgramShadowMap, matModel, sMatCameraV, sMatSceneProj);

    /* Draw floor. */
#define DRAW_FLOOR
#ifdef DRAW_FLOOR
    XMatrixAPI floorMVP = sMatSceneProj * sMatCameraV;
    XMatrixAPI itmv = XMatrixAPI::XMatrixAPIInvert(&sMatCameraV);
    XMatrixAPI::XMatrixAPITranspose(&itmv);

    theProgramShadowMap->use();
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_MAT_M, XMatrixAPI::identityXMatrixAPI);
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_MAT_MV, sMatCameraV);
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_MAT_MV_INV_TRANS, itmv);
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_MAT_MVP, floorMVP);
    theProgramShadowMap->setUniform(MRendererProgram::U_LOC_GENERIC_1, MVector4f(0.0f, 0.0f, 0.0f, 1.0f));

    GLint iLocPosition = theProgramShadowMap->getLocAttrib(MRendererProgram::A_LOC_VERTEX);
    GLint iLocFillColor = theProgramShadowMap->getLocAttrib(MRendererProgram::A_LOC_COLOR);
    GLint iLocNormal = theProgramShadowMap->getLocAttrib(MRendererProgram::A_LOC_NORMAL);

    if (iLocPosition != -1) {
        GL_CHECK(glEnableVertexAttribArray(iLocPosition));
        GL_CHECK(glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, aFloorVertex));
    }
    if (iLocFillColor != -1) {
        GL_CHECK(glEnableVertexAttribArray(iLocFillColor));
        GL_CHECK(glVertexAttribPointer(iLocFillColor, 3, GL_FLOAT, GL_FALSE, 0, aFloorColor));
    }
    if (iLocNormal != -1) {
        GL_CHECK(glEnableVertexAttribArray(iLocNormal));
        GL_CHECK(glVertexAttribPointer(iLocNormal, 3, GL_FLOAT, GL_FALSE, 0, aFloorNormal));
    }
    GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, aFloorIndex));
#endif

#define DRAW_LIGHT
#ifdef DRAW_LIGHT
    /* Transform light position into world space position and draw Light. */
    XMatrixAPI timvp = sMatCameraV * sMatLightM;
    XMatrixAPI mRX90 = XMatrixAPI::createRotationX(50.0f);
    timvp = timvp * mRX90;
    timvp = sMatSceneProj * timvp;

    theProgramSolid->use();
    theProgramSolid->setUniform(MRendererProgram::U_LOC_MAT_MVP, timvp);

    /* Draw light flare. */
    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, theTextureFlare->getName()));

    timvp = XMatrixAPI::createTranslation(lightPos.x, lightPos.y, lightPos.z);
    /* Rotate flare according to angle between viewing point and light. */
    XMatrixAPI mRZF = XMatrixAPI::createRotationZ(theGlobalAmbient[0] * 50.0f);
    timvp = timvp * mRZF;
    timvp = sMatSceneProj * timvp;

    theProgramFlare->use();
    theProgramFlare->setUniform(MRendererProgram::U_LOC_GENERIC_1, MVector4f(1.0f, 1.0f, 1.0f, theGlobalAmbient[0]));
    theProgramFlare->setUniform(MRendererProgram::U_LOC_MAT_MVP, timvp);
    theGeometryRectangle->render(*theProgramFlare);

    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    theRT->bindTexture();
    GL_CHECK(glEnable(GL_CULL_FACE));
#endif
    text->draw();
}

/**
 * @brief Main application entry point for shadow mapping demonstration
 * 
 * @return Exit status code (0 for success)
 * 
 * This application demonstrates advanced shadow mapping techniques using OpenGL ES 2.0:
 * - Multi-pass rendering for shadow map generation and scene rendering
 * - Depth texture usage for shadow calculations
 * - Complex 3D scene with multiple objects casting and receiving shadows
 * - Real-time shadow map updates with dynamic light positioning
 * 
 * The shadow mapping algorithm works in two main passes:
 * 1. Shadow Map Pass: Renders the scene from the light's perspective to generate depth map
 * 2. Lighting Pass: Renders the scene from camera perspective using shadow map for occlusion testing
 * 
 * This creates realistic dynamic shadows that respond to object and light movement,
 * demonstrating professional-grade shadow rendering techniques.
 */
int main(void) {
    /* Intialize the CrossPlatformEngine object for platform specific functions. */
    XPodium *platform = XPodium::getHandler();

    /* Initialize windowing system. */
    platform->prepareWindow(WINDOW_W, WINDOW_H);

    /* Initialize EGL. */
    CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
    EGL_CHECK(eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context));

    /* Initialize OpenGL ES graphics subsystem. */
    setupGraphics(WINDOW_W, WINDOW_H);

    bool end = false;
    /* The rendering loop to draw the scene. */
    while (!end) {
        /* If something has happened to the window, end the sample. */
        if (platform->checkWindow() != XPodium::WINDOW_IDLE) {
            end = true;
        }

        /* Render a single frame */
        renderFrame();

        /*
         * Push the EGL surface color buffer to the native window.
         * Causes the rendered graphics to be displayed on screen.
         */
        eglSwapBuffers(CoreEGL::display, CoreEGL::surface);
    }

    /* Release resources of the sample */
    terminateGraphics();
    delete text;
    /* Shut down OpenGL ES. */
    /* Shut down EGL. */
    CoreEGL::terminateEGL();

    /* Shut down windowing system. */
    platform->destroyWindow();

    /* Shut down the CrossPlatformEngine object*/
    delete platform;

    return 0;
}
