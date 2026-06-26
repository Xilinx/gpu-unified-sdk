/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/

#include "sort.h"
#include "common/matrix.h"
#include "common/glutil.h"
#include "common/shader.h"
#include "common/common.h"
#include <string.h>
#include <vector>

ShaderEngine shader_scanblock, shader_scansums, shader_reorder;

GLuint buf_scan, buf_sums, buf_flag, buf_sorted;

bool sort_init()
{
    std::string res = "./Supportingfiles/shaders/";
    if (!shader_scanblock.load_compute_from_file(res + "scanblock.cs") ||
        !shader_scansums.load_compute_from_file(res + "scansums.cs") ||
        !shader_reorder.load_compute_from_file(res + "reorder.cs"))
    {
        return false;
    }

    if (!shader_scanblock.link() || !shader_scansums.link() || !shader_reorder.link())
    {
        return false;
    }

    std::vector<GLuint> zeroes(4 * NUM_KEYS, 0);

    buf_scan = gen_buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, NUM_KEYS * 4 * sizeof(GLuint), zeroes.data());
    buf_sums = gen_buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, NUM_BLOCKS * 4 * sizeof(GLuint), zeroes.data());
    buf_flag = gen_buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, NUM_KEYS * 4 * sizeof(GLuint), zeroes.data());
    buf_sorted = gen_buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, NUM_KEYS * sizeof(vec4), nullptr);

    return true;
}

void sort_free()
{
    del_buffer(buf_flag);
    del_buffer(buf_scan);
    del_buffer(buf_sums);
    del_buffer(buf_sorted);

    shader_scanblock.dispose();
    shader_scansums.dispose();
    shader_reorder.dispose();
}

void scan_block(GLuint buf_input, uint32_t bit_offset, vec3 axis, float z_min, float z_max)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf_scan);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_sums);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_input);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buf_flag);
    use_shader(shader_scanblock);
    uniform("bitOffset", bit_offset);
    uniform("axis", axis);
    uniform("zMin", z_min);
    uniform("zMax", z_max);
    glDispatchCompute(NUM_BLOCKS, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 0);
}

void scan_sums()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_sums);
    use_shader(shader_scansums);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
}

void reorder(GLuint buf_input)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buf_scan);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buf_sums);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buf_input);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buf_flag);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buf_sorted);
    use_shader(shader_reorder);
    glDispatchCompute(NUM_BLOCKS, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, 0);
}

void radix_sort(GLuint buf_input, vec3 axis, float z_min, float z_max)
{
    for (uint32_t i = 0; i < 8; i++)
    {
        scan_block(buf_input, i * 2, axis, z_min, z_max);
        scan_sums();
        reorder(buf_input);

        // Swap for the next digit stage
        std::swap(buf_input, buf_sorted);
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}
