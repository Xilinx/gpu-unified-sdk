/*
 * Copyright (C) 2024-2026 Advanced Micro Devices, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: MIT
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * \brief Kernel to compute Mandelbrot set escape times with optional image perturbation.
 * \param[in] perturbation Optional input image data to perturb initial conditions (luminance).
 * \param[in] width Width of the output grid.
 * \param[in] height Height of the output grid.
 * \param[out] output Escape time values (uchar, 0-255).
 */
__kernel void compute_mandelbrot(__global const uchar* restrict perturbation,
                                const int width,
                                const int height,
                                __global uchar* restrict output)
{
    const int column = get_global_id(0) * 16;
    const int row = get_global_id(1);
    const int offset = row * width + column;
    const int maxIterations = 100;

    float16 x0 = (float16)((column + (float16)(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)) * 2.5f / width - 2.0f);
    float16 y0 = (float16)(row * 2.0f / height - 1.0f);
    if (perturbation != NULL)
    {
        uchar16 pert = vload16(0, perturbation + offset);
        float16 pertFloat = convert_float16(pert) / 255.0f * 0.1f; // Scale perturbation
        x0 += pertFloat;
        y0 += pertFloat;
    }

    float16 x = x0;
    float16 y = y0;
    float16 x2 = x * x;
    float16 y2 = y * y;
    uchar16 iterations = (uchar16)(0);

    for (int i = 0; i < maxIterations; i++)
    {
        float16 xy = x * y;
        x = x2 - y2 + x0;
        y = xy + xy + y0;
        x2 = x * x;
        y2 = y * y;
        int16 mask = (x2 + y2 < 4.0f) & convert_int16(iterations < (uchar16)(255));
        iterations += convert_uchar16(mask);
        if (!any(mask))
            break;
    }

    if (row < height && column < width)
    {
        vstore16(iterations, 0, output + offset);
    }
}