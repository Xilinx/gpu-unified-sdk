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
 * Bilinear image rescale. One work-item writes one destination pixel: it maps
 * the destination coordinate into normalised [0,1] source space and lets a
 * linear-filtering sampler perform the interpolation.
 */
const sampler_t linearSampler = CLK_NORMALIZED_COORDS_TRUE |
                                CLK_ADDRESS_CLAMP |
                                CLK_FILTER_LINEAR;

__kernel void image_scaling(__read_only  image2d_t sourceImage,
                            __write_only image2d_t destinationImage,
                            const float widthNormalizationFactor,
                            const float heightNormalizationFactor)
{
    const int2 destinationCoordinate = (int2)(get_global_id(0), get_global_id(1));

    const float2 sourceCoordinate =
        (float2)(destinationCoordinate.x * widthNormalizationFactor,
                 destinationCoordinate.y * heightNormalizationFactor);

    const float4 sampledColour =
        read_imagef(sourceImage, linearSampler, sourceCoordinate);

    write_imagef(destinationImage, destinationCoordinate, sampledColour);
}
