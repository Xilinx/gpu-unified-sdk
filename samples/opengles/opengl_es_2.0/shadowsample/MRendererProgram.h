/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 */
/** 
 * 
 * MODIFICATION HISTORY:
 *
 * Ver   Who            Date            Changes
 * ----- ----           --------        ---------------------------------------
 * 1.0   Alok G         10/06/17        Initial release.
 * 2.0   Aidan R		8/20/24			Cmake + Doxygen.
 * 
 *
 *
 * @file MRendererProgram.h
 *
 * @brief  This class represents vertex and fragment programs combined together.
 *
*******************************************************************************/
#ifndef MRENDERERPROGRAM_H
#define MRENDERERPROGRAM_H

#include <string>
#include <vector>
#include "ShadowMap.h"
#include "MArray.h"
#include "MMatrix.h"
#include "MVector4.h"
#include "MString.h"

/**
 * @brief Manages a shader program for rendering.
 * 
 * The MRendererProgram class encapsulates the functionality for creating, managing, and using 
 * an OpenGL shader program. It handles shader compilation, attribute and uniform location 
 * binding, and setting uniform values in the shader program.
 */
class MRendererProgram {
public:
    // ----- Types -----

    /**
     * @brief Enumeration of attribute locations in the shader program.
     * 
     * Defines different vertex attribute locations used in the shader program.
     */
    enum MDRAttribLocation {
        A_LOC_VERTEX,           ///< Location for vertex attributes.
        A_LOC_NORMAL,           ///< Location for normal attributes.
        A_LOC_COLOR,            ///< Location for color attributes.
        A_LOC_TEXCOORD_0,       ///< Location for the first set of texture coordinates.
        A_LOC_TEXCOORD_1,       ///< Location for the second set of texture coordinates.
        A_LOC_TEXCOORD_2,       ///< Location for the third set of texture coordinates.
        A_LOC_TEXCOORD_3,       ///< Location for the fourth set of texture coordinates.
        A_LOC_TEXCOORD_4,       ///< Location for the fifth set of texture coordinates.
        A_LOC_TEXCOORD_5,       ///< Location for the sixth set of texture coordinates.
        A_LOC_TEXCOORD_6,       ///< Location for the seventh set of texture coordinates.
        A_LOC_TEXCOORD_7,       ///< Location for the eighth set of texture coordinates.
        ATTRIBS_COUNT           ///< Total number of attribute locations.
    };

   /**
     * @brief Enumeration of uniform locations in the shader program.
     * 
     * Defines different uniform locations used in the shader program.
     */
    enum MUniformLocation {
        U_LOC_MAT_MVP,           ///< Location for the Model-View-Projection matrix.
        U_LOC_MAT_MV,            ///< Location for the Model-View matrix.
        U_LOC_MAT_M,             ///< Location for the Model matrix.
        U_LOC_MAT_V,             ///< Location for the View matrix.
        U_LOC_MAT_P,             ///< Location for the Projection matrix.
        U_LOC_MAT_MV_INV,        ///< Location for the inverse Model-View matrix.
        U_LOC_MAT_MV_INV_TRANS,  ///< Location for the inverse-transpose Model-View matrix.
        U_LOC_MAT_0,             ///< Location for the first generic matrix.
        U_LOC_MAT_1,             ///< Location for the second generic matrix.
        U_LOC_MAT_2,             ///< Location for the third generic matrix.
        U_LOC_MAT_3,             ///< Location for the fourth generic matrix.
        U_LOC_MAT_4,             ///< Location for the fifth generic matrix.
        U_LOC_MAT_5,             ///< Location for the sixth generic matrix.
        U_LOC_MAT_6,             ///< Location for the seventh generic matrix.
        U_LOC_MAT_7,             ///< Location for the eighth generic matrix.
        U_LOC_SAMPLER_0,         ///< Location for the first texture sampler.
        U_LOC_SAMPLER_1,         ///< Location for the second texture sampler.
        U_LOC_SAMPLER_2,         ///< Location for the third texture sampler.
        U_LOC_SAMPLER_3,         ///< Location for the fourth texture sampler.
        U_LOC_SAMPLER_4,         ///< Location for the fifth texture sampler.
        U_LOC_GENERIC_1,         ///< Location for the first generic uniform.
        U_LOC_GENERIC_2,         ///< Location for the second generic uniform.
        U_LOC_GENERIC_3,         ///< Location for the third generic uniform.
        U_LOC_GENERIC_4,         ///< Location for the fourth generic uniform.
        U_LOC_GENERIC_5,         ///< Location for the fifth generic uniform.
        U_LOC_GENERIC_6,         ///< Location for the sixth generic uniform.
        U_LOC_GENERIC_7,         ///< Location for the seventh generic uniform.
        U_LOC_GENERIC_8,         ///< Location for the eighth generic uniform.
        U_LOC_GENERIC_9,         ///< Location for the ninth generic uniform.
        U_LOC_GENERIC_10,        ///< Location for the tenth generic uniform.
        U_LOC_GENERIC_11,        ///< Location for the eleventh generic uniform.
        U_LOC_GENERIC_12,        ///< Location for the twelfth generic uniform.
        U_LOC_GENERIC_13,        ///< Location for the thirteenth generic uniform.
        U_LOC_GENERIC_14,        ///< Location for the fourteenth generic uniform.
        U_LOC_GENERIC_15,        ///< Location for the fifteenth generic uniform.
        UNIFORMS_COUNT           ///< Total number of uniform locations.
    };

/**
     * @brief Type definition for uniform and attribute location values.
     * 
     * Defines the type used for storing attribute and uniform locations.
     */
    typedef GLint MDRLocValueType;

    // ----- Constructors and destructors -----

    /**
     * @brief Default constructor for MRendererProgram.
     * 
     * Initializes an MRendererProgram instance with default values.
     */
    MRendererProgram();

      /**
     * @brief Destructor for MRendererProgram.
     * 
     * Cleans up resources associated with the shader program.
     */
    ~MRendererProgram();

 /**
     * @brief Retrieves the location of a vertex attribute.
     * 
     * Gets the location of a specified vertex attribute in the shader program.
     * 
     * @param aAttribLocationType The type of attribute location to retrieve.
     * 
     * @return The location of the specified attribute.
     */
   MDRLocValueType getLocAttrib(MDRAttribLocation aAttribLocationType) const {
        return theLocAttrib[aAttribLocationType];
    }
    /**
     * @brief Retrieves the location of a uniform variable.
     * 
     * Gets the location of a specified uniform variable in the shader program.
     * 
     * @param aUniformLocationType The type of uniform location to retrieve.
     * 
     * @return The location of the specified uniform variable.
     */
     MDRLocValueType getLocUniform(MUniformLocation aUniformLocationType) const {
        return theLocUniform[aUniformLocationType];
    }

 // ----- Miscellaneous -----

    /**
     * @brief Initializes the shader program with vertex and fragment shaders.
     * 
     * Loads and compiles the vertex and fragment shaders, and links them into a shader program.
     * 
     * @param vertexShaderPath Path to the vertex shader source file.
     * @param fragmentShaderPath Path to the fragment shader source file.
     * 
     * @return True if the initialization is successful, false otherwise.
     */
    bool initialize(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    /**
     * @brief Binds a vertex attribute location to a name in the shader program.
     * 
     * Associates a vertex attribute location with a specified attribute name.
     * 
     * @param aAttribLocationType The attribute location to bind.
     * @param aAttribName The name of the vertex attribute in the shader program.
     */
    void bindAttrib(MDRAttribLocation aAttribLocationType, const char* aAttribName);

    /**
     * @brief Binds a uniform location to a name in the shader program.
     * 
     * Associates a uniform location with a specified uniform name.
     * 
     * @param aUniformLocationType The uniform location to bind.
     * @param aUniformName The name of the uniform variable in the shader program.
     */
    void bindUniform(MUniformLocation aUniformLocationType, const char* aUniformName);

    /**
     * @brief Activates the shader program for rendering.
     * 
     * Binds the shader program so that it can be used for rendering.
     */
    void use() const;

    /**
     * @brief Sets a matrix uniform value in the shader program.
     * 
     * Updates the value of a matrix uniform variable in the shader program.
     * 
     * @param aUniformLocationType The location of the uniform variable.
     * @param matrix The matrix value to set.
     */
    void setUniform(MUniformLocation aUniformLocationType, const XMatrixAPI& matrix) const;

    /**
     * @brief Sets a matrix uniform value in the shader program.
     * 
     * Updates the value of a matrix uniform variable in the shader program.
     * 
     * @param aUniformLocationType The location of the uniform variable.
     * @param matrix The matrix value to set.
     */
    void setUniform(MUniformLocation aUniformLocationType, const MMatrix4f& matrix) const;

    /**
     * @brief Sets a vector uniform value in the shader program.
     * 
     * Updates the value of a vector uniform variable in the shader program.
     * 
     * @param aUniformLocationType The location of the uniform variable.
     * @param vector The vector value to set.
     */
    void setUniform(MUniformLocation aUniformLocationType, const MVector4f& vector) const;

    /**
     * @brief Sets a vector uniform value in the shader program.
     * 
     * Updates the value of a vector uniform variable in the shader program.
     * 
     * @param aUniformLocationType The location of the uniform variable.
     * @param vector The vector value to set.
     */
    void setUniform(MUniformLocation aUniformLocationType, const CVec4f& vector) const;

    /**
     * @brief Sets an integer uniform value in the shader program.
     * 
     * Updates the value of an integer uniform variable in the shader program.
     * 
     * @param aUniformLocationType The location of the uniform variable.
     * @param value The integer value to set.
     */
    void setUniform(MUniformLocation aUniformLocationType, int value) const;

private:
    typedef std::vector<MDRLocValueType> MDRLocArray; ///< Type definition for an array of location values.

    GLuint theProgram;               ///< Handle for the shader program.
    GLuint theProgramVertex;         ///< Handle for the vertex shader.
    GLuint theProgramFragment;       ///< Handle for the fragment shader.
    MDRLocArray theLocAttrib;        ///< Array of attribute locations.
    MDRLocArray theLocUniform;       ///< Array of uniform locations.
};

#endif // MRENDERERPROGRAM_H
