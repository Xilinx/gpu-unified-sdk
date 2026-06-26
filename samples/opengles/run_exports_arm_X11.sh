#! /bin/bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.

#
# Runtime environment for the OpenGL ES samples on the aarch64 target using the
# X11 windowing backend.
#
# Source this on the target (or in your run environment) before launching a
# sample that was built with -D SDK_Platform=x11:
#
#     source run_exports_arm_X11.sh
#     ./trianglesample

# Mali user-space driver location (libEGL / libGLESv2 live here on the target).
export MALI_LIBS=/usr/lib
export MALI_INCLUDE=/usr/include
export LD_LIBRARY_PATH=/usr/lib/:${LD_LIBRARY_PATH}

# Select the X11 EGL platform at runtime and point at the running X server.
export EGL_PLATFORM=x11
export DISPLAY=${DISPLAY:-:0.0}
