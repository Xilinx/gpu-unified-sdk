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
 * Demonstrates 64-bit integer arithmetic and atomics. Each work-item reads
 * eight 8-bit pixels, then atomically accumulates both their sum and the sum of
 * their squares into 64-bit global accumulators. The host launches a 1-D range
 * of (pixelCount / 8) work-items.
 */
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable

__kernel void long_vectors(__global uchar* restrict imagePixels,
                           __global ulong* restrict squareOfPixels,
                           __global ulong* restrict sumOfPixels)
{
    const size_t block = get_global_id(0);
    const ushort8 pixels = convert_ushort8(vload8(block, imagePixels));

    ulong runningSum    = 0;
    ulong runningSquare = 0;
    for (int lane = 0; lane < 8; ++lane)
    {
        const ulong value = (ulong)pixels[lane];
        runningSum    += value;
        runningSquare += value * value;
    }

    atom_add(sumOfPixels,    runningSum);
    atom_add(squareOfPixels, runningSquare);
}
