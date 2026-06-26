/*
 * Copyright (C) 2024-2025 Advanced Micro Devices, Inc.  All rights reserved.
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
 * Escape-time Mandelbrot set evaluation. The output byte for each pixel is the
 * iteration count at which the orbit escaped (clamped at MANDELBROT_MAX_ITER,
 * which marks points considered inside the set). Each work-item produces four
 * adjacent pixels on one row; host range is (width / 4, height).
 */
#define MANDELBROT_MAX_ITER 255

__kernel void mandelbrot(__global uchar* restrict output,
                         const int width,
                         const int height)
{
    const int firstX = get_global_id(0) * 4;
    const int y      = get_global_id(1);

    for (int pixel = 0; pixel < 4; ++pixel)
    {
        const int x = firstX + pixel;

        /* Map the pixel onto the classic [-2,0.5] x [-1,1] viewing window. */
        const float cReal = -2.0f + ((float)x / (float)width)  * 2.5f;
        const float cImag = -1.0f + ((float)y / (float)height) * 2.0f;

        float zReal = cReal;
        float zImag = cImag;
        int   iteration = 0;

        while (iteration < MANDELBROT_MAX_ITER)
        {
            const float zRealSq = zReal * zReal;
            const float zImagSq = zImag * zImag;
            if (zRealSq + zImagSq > 4.0f)
            {
                break;
            }

            zImag = 2.0f * zReal * zImag + cImag;
            zReal = zRealSq - zImagSq + cReal;
            ++iteration;
        }

        output[y * width + x] = (uchar)iteration;
    }
}
