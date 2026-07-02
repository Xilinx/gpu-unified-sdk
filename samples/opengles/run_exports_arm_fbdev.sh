#! /bin/bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.

#
# Runtime environment for the OpenGL ES samples on the aarch64 target using the
# framebuffer (fbdev) windowing backend.
#
# Source this on the target (or in your run environment) before launching a
# sample that was built with -D SDK_Platform=fbdev:
#
#     source run_exports_arm_fbdev.sh
#     ./trianglesample

# Mali user-space driver location (libEGL / libGLESv2 live here on the target).
export MALI_LIBS=/usr/lib
export MALI_INCLUDE=/usr/include
export LD_LIBRARY_PATH=/usr/lib/:${LD_LIBRARY_PATH}

# Select the framebuffer EGL platform at runtime.
export EGL_PLATFORM=fbdev
