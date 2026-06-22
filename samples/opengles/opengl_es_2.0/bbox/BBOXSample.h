
#define GLES_VERSION 2

#include <GLES2/gl2.h>
#include <string>

using namespace std;

#define SCENE_WIDTH 800
#define SCENE_HEIGHT 600

string resourceDirectory = "Supportingfiles/";
string vertexShaderFilename = "BBOX.vert";
string fragmentShaderFilename = "BBOX.frag";

GLuint programID;
GLint iLocPosition = -1;
GLint iLocFillColor = -1;
GLint iLocMVP = -1;

const float triangleVertices[] =
{
     0.0f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
};

const float triangleColors[] =
{
    1.0, 1.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 1.0, 1.0,
};
                             
