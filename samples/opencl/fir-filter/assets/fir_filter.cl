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

/*
 * Separable-free 3x3 FIR (finite impulse response) image filter.
 *
 * The nine filter taps below are fixed coefficients (1/256 fixed-point weights)
 * that define the response of this particular filter. Each work-item produces
 * four consecutive output pixels on one row; the host therefore launches a 2-D
 * range of (width / 4, height).
 */
#define FIR_INV_SCALE (1.0f / 256.0f)

__kernel void fir_float(__global const float* restrict input,
                        __global float* restrict output,
                        const int width)
{
    /* Row-major 3x3 tap weights. */
    const float taps[9] = { 30.0f,  5.0f,  6.0f,
                            19.0f, 30.0f,  9.0f,
                            15.0f,  5.0f, 40.0f };

    const int firstColumn = get_global_id(0) * 4;
    const int row         = get_global_id(1);

    for (int pixel = 0; pixel < 4; ++pixel)
    {
        const int column = firstColumn + pixel;
        const int anchor = row * width + column;

        float accumulator = 0.0f;
        for (int ky = 0; ky < 3; ++ky)
        {
            const int rowBase = anchor + ky * width;
            accumulator += input[rowBase + 0] * taps[ky * 3 + 0];
            accumulator += input[rowBase + 1] * taps[ky * 3 + 1];
            accumulator += input[rowBase + 2] * taps[ky * 3 + 2];
        }

        output[anchor] = accumulator * FIR_INV_SCALE;
    }
}
