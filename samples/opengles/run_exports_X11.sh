#! /bin/bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.

#
# Runtime environment for the OpenGL ES samples on the target using the
# X11 windowing backend.
#
# Source this on the target (or in your run environment) before launching a
# sample that was built with -D SDK_Platform=x11:
#
#     source run_exports_X11.sh
#     ./trianglesample

# Point at the running X server.
export DISPLAY=:0.0
