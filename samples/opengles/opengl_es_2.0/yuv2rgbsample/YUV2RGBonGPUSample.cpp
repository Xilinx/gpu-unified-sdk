/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/******************************************************************************/
/**
 *
 * @file YUV2RGBonGPUSample.cpp
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
 * </pre>
 *
 *******************************************************************************/
/******************************* Source Files ********************************/


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "XPodium.h"
#include "YUV2RGBonGPUSample.h"
#include "XGLSLCompile.h"
#include "XEGLIntf.h"
#include "XMatrixAPI.h"
#include "string"



#ifndef WINDOW_W
#define WINDOW_W 1920
#endif

#ifndef WINDOW_H
#define WINDOW_H 1080
#endif

using std::string;

string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "YUV2RGBonGPUSample.vert";
string fragmentShaderFilename = "YUV2RGBonGPUSample.frag";

GLuint programID;
GLint iLocPosition = -1;
GLint itimeLocation = -1;
GLint iLocMVP = -1;
GLint iLocTexture = -1;

//YUV file
GLuint id_y, id_u, id_v; // Texture id
GLuint textureUniformY, textureUniformU, textureUniformV;

const int pixel_w = WINDOW_W, pixel_h = WINDOW_H;
FILE *infile = NULL;
unsigned char buf[pixel_w*pixel_h * 3 / 2];
unsigned char *plane[3];

/**
 * @brief Initializes graphics resources for YUV to RGB color space conversion demonstration
 * 
 * @param width Window width in pixels
 * @param height Window height in pixels
 * @return true if initialization successful, false otherwise
 * 
 * This function sets up a GPU-accelerated YUV to RGB color space conversion pipeline:
 * - Configures shaders for YUV420 to RGB conversion using fragment shader
 * - Sets up texture units for Y, U, and V color planes
 * - Initializes uniform locations for color conversion matrices
 * - Prepares vertex attributes for quad rendering
 * 
 * The YUV to RGB conversion is performed entirely on the GPU using shader-based
 * color space transformation, which is much more efficient than CPU-based conversion
 * for video processing applications.
 */
bool prepareGraphics(int width, int height)
{
	printf("setupGraphics(%d, %d)", width, height);
	string vertexShaderPath = resourceDirectory + vertexShaderFilename; 
	string fragmentShaderPath = resourceDirectory + fragmentShaderFilename;

	GLuint vertexShaderID = 0;
	GLuint fragmentShaderID = 0;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	iLocPosition = glGetAttribLocation(programID, "av4position");
	iLocTexture = glGetAttribLocation(programID, "av3colour");
	iLocMVP = glGetUniformLocation(programID, "mvp");
	textureUniformY = glGetUniformLocation(programID, "tex_y");
	textureUniformU = glGetUniformLocation(programID, "tex_u");
	textureUniformV = glGetUniformLocation(programID, "tex_v");

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram (programID);
	glEnableVertexAttribArray(iLocPosition);
	glEnableVertexAttribArray(iLocTexture);
	glVertexAttribPointer(iLocPosition, 2, GL_FLOAT, GL_FALSE, 0, vertexVertices);
	glVertexAttribPointer(iLocTexture, 2, GL_FLOAT, GL_FALSE, 0, textureVertices);

	glGenTextures(1, &id_y);
	glBindTexture(GL_TEXTURE_2D, id_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &id_u);
	glBindTexture(GL_TEXTURE_2D, id_u);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &id_v);
	glBindTexture(GL_TEXTURE_2D, id_v);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	return true;
}

/**
 * @brief Graphics rendering function for YUV to RGB conversion demonstration
 * 
 * @param time Current time value for animation (currently unused)
 * 
 * This function performs real-time YUV to RGB color space conversion:
 * - Reads YUV420 video frame data from file
 * - Uploads Y, U, and V planes to separate GPU textures
 * - Applies 3D transformation matrices for rotating display
 * - Renders textured quad with GPU-based YUV-to-RGB conversion
 * 
 * The conversion process:
 * 1. Loads raw YUV420 video data (Y plane + UV interleaved)
 * 2. Separates Y, U, V components into individual textures
 * 3. Fragment shader performs color space conversion using standard matrices
 * 4. Results in full-color RGB output displayed on rotating quad
 * 
 * This demonstrates efficient video processing pipeline suitable for
 * real-time video playback and processing applications.
 */
void GraphicsUpdate(float time)
{

	static float angleX = 0, angleY = 0, angleZ = 0;
	XMatrixAPI modelView = XMatrixAPI::createRotationX(angleX);
	XMatrixAPI rotation = XMatrixAPI::createRotationY(angleY);
	modelView = rotation * modelView;
	modelView[14] -= 1.5;

	XMatrixAPI perspective = XMatrixAPI::XMatrixAPIPerspective(45.0f, 800/600, 0.01f, 100.0f);
	XMatrixAPI modelViewPerspective = perspective * modelView;

	glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, modelViewPerspective.getAsArray());


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	size_t frameSize = pixel_w*pixel_h * 3 / 2;
	if (fread(buf, 1, frameSize, infile) != frameSize) {
		// End of file (single-frame input): rewind and re-read the frame so the
		// sample keeps looping the same image.
		fseek(infile, 0, SEEK_SET);
		if (fread(buf, 1, frameSize, infile) != frameSize) {
			printf("WARNING: short read on YUV input; frame may be incomplete.\n");
		}
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Y
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id_y);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w, pixel_h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[0]);
	glUniform1i(textureUniformY, 0);
	//U
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, id_u);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w / 2, pixel_h / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[1]);
	glUniform1i(textureUniformU, 1);
	//V
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, id_v);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pixel_w / 2, pixel_h / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, plane[2]);
	glUniform1i(textureUniformV, 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}


static void print_usage(const char* prog)
{
	const char* name = prog ? prog : "yuv2rgbsample";

	printf("\n");
	printf("yuv2rgbsample - YUV420P (I420) to RGB colour conversion on the GPU.\n");
	printf("\n");
	printf("Usage: %s <input.yuv>\n", name);
	printf("       %s [-h|--help]\n", name);
	printf("\n");
	printf("Arguments:\n");
	printf("  <input.yuv>   Path to the raw YUV frame to load and display.\n");
	printf("                This argument is required; there is no default file.\n");
	printf("\n");
	printf("Input requirements:\n");
	printf("  - format: raw YUV420P / I420 (planar Y, then U, then V; no header)\n");
	printf("  - size  : %dx%d, i.e. %d bytes per frame\n",
	       pixel_w, pixel_h, pixel_w * pixel_h * 3 / 2);
	printf("\n");
	printf("Generate a test pattern (color test card) with ffmpeg:\n");
	printf("  ffmpeg -y -f lavfi -i testsrc=size=%dx%d:rate=1 -frames:v 1 \\\n",
	       pixel_w, pixel_h);
	printf("         -pix_fmt yuv420p -f rawvideo input.yuv\n");
	printf("\n");
	printf("Or convert an existing image / video frame:\n");
	printf("  ffmpeg -i input.png -vf scale=%d:%d -pix_fmt yuv420p -f rawvideo input.yuv\n",
	       pixel_w, pixel_h);
	printf("\n");
	printf("Then run:\n");
	printf("  %s input.yuv\n", name);
	printf("\n");
}

int main(int argc, char* argv[])
{
	const char* rawyuvpath = NULL;

	for (int a = 1; a < argc; ++a) {
		if (strcmp(argv[a], "-h") == 0 || strcmp(argv[a], "--help") == 0) {
			print_usage(argv[0]);
			return 0;
		}
		if (argv[a][0] == '-') {
			printf("ERROR: unknown option '%s'.\n", argv[a]);
			print_usage(argv[0]);
			return -1;
		}
		// First positional argument is the input YUV file path.
		if (rawyuvpath == NULL) {
			rawyuvpath = argv[a];
		}
	}

	// The input file path must be provided by the user; there is no default.
	if (rawyuvpath == NULL) {
		printf("\n");
		printf("ERROR: no input YUV file provided.\n");
		print_usage(argv[0]);
		return -1;
	}

	string rawyuvfile = rawyuvpath;

	// Open the user-provided YUV420P file before bringing up the window so a
	// missing/invalid path fails fast with a clear message.
	if ((infile = fopen(rawyuvfile.c_str(), "rb")) == NULL) {
		printf("\n");
		printf("ERROR: could not open input YUV file '%s'.\n", rawyuvfile.c_str());
		print_usage(argv[0]);
		return -1;
	}

	XPodium *podium = XPodium::getHandler();
	podium->prepareWindow(WINDOW_W, WINDOW_H);
	CoreEGL::initializeEGL(CoreEGL::OPENGLES2);
	eglMakeCurrent(CoreEGL::display, CoreEGL::surface, CoreEGL::surface, CoreEGL::context);

	//YUV Data
	plane[0] = buf;
	plane[1] = plane[0] + pixel_w*pixel_h;
	plane[2] = plane[1] + pixel_w*pixel_h / 4;

	prepareGraphics(WINDOW_W, WINDOW_H);

	bool end = false;
	float i = 0.0f;
	while(!end)
	{
		if(podium->checkWindow() != XPodium::WINDOW_IDLE)
		{
			end = true;
		}
		GraphicsUpdate(i);
		eglSwapBuffers(CoreEGL::display, CoreEGL::surface);

		if(i<=10000)
			i= i + 0.04f;
		else
			i=0;

	}
	CoreEGL::terminateEGL();
	podium->destroyWindow();
	return 0;
}

/**
 * @brief Main application entry point for YUV to RGB conversion demonstration
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Exit status code (0 for success, -1 for file error)
 * 
 * This application demonstrates efficient GPU-based YUV to RGB color space conversion:
 * - Opens and reads raw YUV420P video file data
 * - Sets up OpenGL ES context and graphics pipeline
 * - Continuously processes video frames with GPU-accelerated conversion
 * - Displays converted RGB video with 3D rotation effects
 * 
 * The application showcases:
 * - Real-time video processing capabilities
 * - Efficient GPU memory usage for video textures  
 * - Color space conversion using fragment shaders
 * - File-based video playback with automatic looping
 * 
 * This technique is essential for video playback applications, media players,
 * and any system that needs to process YUV video data efficiently.
 */


