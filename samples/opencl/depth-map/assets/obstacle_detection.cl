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

__kernel void compute_disparity(__global const uchar* restrict leftImage,
                               __global const uchar* restrict rightImage,
                               const int width,
                               const int height,
                               __global uchar* restrict disparityMap,
                               const int maxDisparity,
                               const int windowSize)
{
    const int column = get_global_id(0) * 16;
    const int row = get_global_id(1);
    const int offset = row * width + column;
    const int halfWindow = windowSize / 2;

    uchar disparitiesArr[16] = {0};

    if (row >= halfWindow && row < height - halfWindow && column >= halfWindow && column < width - halfWindow - maxDisparity)
    {
        for (int i = 0; i < 16 && column + i < width - halfWindow; i++)
        {
            int minSAD = 65535;
            int bestDisparity = 0;

            for (int d = 0; d <= maxDisparity; d++)
            {
                int sad = 0;
                for (int wy = -halfWindow; wy <= halfWindow; wy++)
                {
                    for (int wx = -halfWindow; wx <= halfWindow; wx++)
                    {
                        int leftIdx = (row + wy) * width + (column + i + wx);
                        int rightIdx = (row + wy) * width + (column + i + wx - d);
                        int diff = abs((int)leftImage[leftIdx] - (int)rightImage[rightIdx]);
                        sad += diff;
                    }
                }
                if (sad < minSAD)
                {
                    minSAD = sad;
                    bestDisparity = d;
                }
            }
            disparitiesArr[i] = (uchar)(bestDisparity * 255 / maxDisparity);
        }
    }

    int validCount = (width - column - halfWindow - maxDisparity) > 16 ? 16 : (width - column - halfWindow - maxDisparity);
    if (validCount > 0 && row < height && column < width)
    {
        uchar16 disparitiesVec = (uchar16)(disparitiesArr[0], disparitiesArr[1], disparitiesArr[2], disparitiesArr[3],
                                          disparitiesArr[4], disparitiesArr[5], disparitiesArr[6], disparitiesArr[7],
                                          disparitiesArr[8], disparitiesArr[9], disparitiesArr[10], disparitiesArr[11],
                                          disparitiesArr[12], disparitiesArr[13], disparitiesArr[14], disparitiesArr[15]);

        vstore16(disparitiesVec, 0, disparityMap + offset);
    }
}

