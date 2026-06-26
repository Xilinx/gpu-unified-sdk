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
 * 3x3 Sobel edge filter producing separate signed X and Y gradient images.
 * Each work-item processes 16 horizontally adjacent pixels (host range is
 * (width / 16, height)), iterating over the lanes scalar-wise for clarity.
 */
__kernel void sobel(__global const uchar* restrict inputImage,
                    const int width,
                    __global char* restrict outputImageDX,
                    __global char* restrict outputImageDY)
{
    const int firstColumn = get_global_id(0) * 16;
    const int row         = get_global_id(1);

    for (int lane = 0; lane < 16; ++lane)
    {
        const int column = firstColumn + lane;
        const int origin = row * width + column;

    /* Sample the 3x3 neighbourhood anchored at the top-left ('origin'). */
        const short topLeft     = (short)inputImage[origin];
        const short topMiddle   = (short)inputImage[origin + 1];
        const short topRight    = (short)inputImage[origin + 2];
        const short midLeft     = (short)inputImage[origin + width];
        const short midRight    = (short)inputImage[origin + width + 2];
        const short botLeft     = (short)inputImage[origin + 2 * width];
        const short botMiddle   = (short)inputImage[origin + 2 * width + 1];
        const short botRight    = (short)inputImage[origin + 2 * width + 2];

        /* Standard Sobel horizontal (gx) and vertical (gy) gradients. */
        const short gx = (topRight - topLeft)
                       + 2 * (midRight - midLeft)
                       + (botRight - botLeft);
        const short gy = (topRight + topLeft + 2 * topMiddle)
                       - (botRight + botLeft + 2 * botMiddle);

        /* Reduce the [-1020,1020] range to a signed byte; store at the centre. */
        outputImageDX[origin + width + 1] = convert_char(gx >> 3);
        outputImageDY[origin + width + 1] = convert_char(gy >> 3);
    }
}
