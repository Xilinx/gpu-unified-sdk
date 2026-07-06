/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "MRendererProgram.h"

using namespace SDKAMD;

MRendererProgram::MRendererProgram()
    : theProgram(0),
      theProgramVertex(0),
      theProgramFragment(0),
      theLocAttrib(),
      theLocUniform() {
    theLocAttrib.resize(ATTRIBS_COUNT, -1);
    theLocUniform.resize(UNIFORMS_COUNT, -1);
}

MRendererProgram::~MRendererProgram() {
    // Cleanup shaders and program
    glDeleteProgram(theProgram);
    glDeleteShader(theProgramVertex);
    glDeleteShader(theProgramFragment);
}

bool MRendererProgram::initialize(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    // Shader initialization
    Shader::processShader(&theProgramVertex, vertexShaderPath.c_str(), GL_VERTEX_SHADER);
    Shader::processShader(&theProgramFragment, fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);

    // Create theProgram (ready to attach shaders)
    theProgram = GL_CHECK(glCreateProgram());

    // Attach shaders and link theProgram
    GL_CHECK(glAttachShader(theProgram, theProgramVertex));
    GL_CHECK(glAttachShader(theProgram, theProgramFragment));
    GL_CHECK(glLinkProgram(theProgram));
    GL_CHECK(glUseProgram(theProgram));

    return true;
}

void MRendererProgram::bindAttrib(MDRAttribLocation attribLocationType, const char* attribName) {
    theLocAttrib[attribLocationType] = GL_CHECK(glGetAttribLocation(theProgram, attribName));
    LOGD("%s = %i\n", attribName, theLocAttrib[attribLocationType]);
}

void MRendererProgram::bindUniform(MUniformLocation uniformLocationType, const char* uniformName) {
    theLocUniform[uniformLocationType] = GL_CHECK(glGetUniformLocation(theProgram, uniformName));
    LOGD("%s = %i\n", uniformName, theLocUniform[uniformLocationType]);
}

void MRendererProgram::use() const {
    GL_CHECK(glUseProgram(theProgram));
}

void MRendererProgram::setUniform(MUniformLocation uniformLocationType, const XMatrixAPI& matrix) const {
    GLint loc = theLocUniform[uniformLocationType];
    if (loc == -1) return;
    XMatrixAPI copy = matrix;
    GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, copy.getAsArray()));
}

void MRendererProgram::setUniform(MUniformLocation uniformLocationType, const MMatrix4f& matrix) const {
    GLint loc = theLocUniform[uniformLocationType];
    if (loc == -1) return;
    GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.getData()));
}

void MRendererProgram::setUniform(MUniformLocation uniformLocationType, const MVector4f& vector) const {
    GLint loc = theLocUniform[uniformLocationType];
    if (loc == -1) return;
    GL_CHECK(glUniform4fv(loc, 1, (const GLfloat*)vector.getData()));
}

void MRendererProgram::setUniform(MUniformLocation uniformLocationType, const CVec4f& vector) const {
    GLint loc = theLocUniform[uniformLocationType];
    if (loc == -1) return;
    GL_CHECK(glUniform4fv(loc, 1, (const GLfloat*)&vector));
}

void MRendererProgram::setUniform(MUniformLocation uniformLocationType, int value) const {
    GLint loc = theLocUniform[uniformLocationType];
    if (loc == -1) return;
    GL_CHECK(glUniform1i(loc, value));
}
