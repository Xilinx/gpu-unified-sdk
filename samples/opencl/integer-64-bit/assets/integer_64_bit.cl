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
    /*
     * Set i to be the ID of the kernel instance.
     * If the global work size (set by clEnqueueNDRangeKernel) is n,
     * then n kernels will be run and i will be in the range [0, n - 1].
     */
    int i = get_global_id(0);

    /* Load 8 pixels (char) and convert them to shorts to calculate the square. */
    ushort8 pixelShort = convert_ushort8(vload8(i, imagePixels));
    /* Square of 255 < 2 ^ 16. */
    ushort8 newSquareShort = pixelShort * pixelShort;

    /*
     * Convert original pixel value and the square to longs to sum
     * all the vectors together and add the final values to the
     * respective accumulators.
     */
    ulong8 pixelLong = convert_ulong8(pixelShort);
    ulong8 newSquareLong = convert_ulong8(newSquareShort);

    /*
     * Use vector data type suffixes (.lo and .hi) to get smaller vector types,
     * until we obtain one single value. This avoids indexing a vector with a
     * runtime variable, which is not portable across OpenCL compilers.
     */
    ulong4 sumLongPixels1 = pixelLong.hi + pixelLong.lo;
    ulong2 sumLongPixels2 = sumLongPixels1.hi + sumLongPixels1.lo;
    ulong sumLongPixels3 = sumLongPixels2.hi + sumLongPixels2.lo;

    ulong4 sumLongSquares1 = newSquareLong.hi + newSquareLong.lo;
    ulong2 sumLongSquares2 = sumLongSquares1.hi + sumLongSquares1.lo;
    ulong sumLongSquares3 = sumLongSquares2.hi + sumLongSquares2.lo;

    atom_add(sumOfPixels, sumLongPixels3);
    atom_add(squareOfPixels, sumLongSquares3);
}
