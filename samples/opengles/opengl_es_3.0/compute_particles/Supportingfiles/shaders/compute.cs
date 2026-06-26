#version 310 es


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
layout (local_size_x = 8) in;
layout (std140, binding = 0) buffer Data {
    float data[];
};

void main()
{
    data[gl_GlobalInvocationID.x] = 1.0;
}
