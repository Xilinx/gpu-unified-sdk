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
 * Single-precision general matrix multiply for square matrices:
 *     C = alpha * (A * B) + beta * C
 * One work-item computes one element of C; the host launches a 2-D range of
 * (matrixOrder, matrixOrder).
 */
__kernel void sgemm(__global float* restrict matrixA,
                    __global float* restrict const matrixB,
                    __global float* restrict matrixC,
                    const uint matrixOrder,
                    const float alpha,
                    const float beta)
{
    const int row    = get_global_id(1);
    const int column = get_global_id(0);

    float dotProduct = 0.0f;
    for (uint k = 0; k < matrixOrder; ++k)
    {
        dotProduct += matrixA[row * matrixOrder + k] *
                      matrixB[k * matrixOrder + column];
    }

    const uint target = row * matrixOrder + column;
    matrixC[target] = alpha * dotProduct + beta * matrixC[target];
}
