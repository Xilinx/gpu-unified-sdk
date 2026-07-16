#! /bin/bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.

#
# Runtime environment for the OpenGL ES samples on target using the
# framebuffer (fbdev) windowing backend.
#
# Source this on the target (or in your run environment) before launching a
# sample that was built with -D SDK_Platform=fbdev:
#
#     source run_exports_fbdev.sh
#     ./trianglesample


export DISPLAY=:0.0
