# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


# OpenGL ES samples
#
# Samples are grouped by GLES version, with lowercased folder/target names.
# Each sample has a self-contained CMakeLists.txt (explicit add_executable +
# link + asset staging).
#
# All samples build on the SdkGlesFramework (lib/sdk/GlesFramework):
# the OpenGL ES core framework (EGL + Wayland windowing + GLSL compile + matrix /
# font / texture helpers).

# Window size, passed to samples as WINDOW_W / WINDOW_H compile definitions
# (default 1920x1080).
set(GLES_WINDOW_W "1920" CACHE STRING "OpenGL ES sample window width")
set(GLES_WINDOW_H "1080" CACHE STRING "OpenGL ES sample window height")

# Shared FontShaders folder (font.vert / font.frag / font.raw) used by the font
# engine and several samples. Samples stage this next to their binary.
set(GLES_FONT_SHADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../lib/sdk/GlesFramework/FontShaders")

# GLES version sub-selection. On ZynqMP (Mali 400) only the OpenGL ES 2.0
# samples are built; on Versal 2VE/2VM (Mali G78AE) both 2.0 and 3.0 are built. The
# SDK_BUILD_OPENGLES_2_0 / _3_0 switches (top-level CMakeLists) control this and
# can be overridden explicitly.
if (SDK_BUILD_OPENGLES_2_0)
	add_subdirectory(opengl_es_2.0)
endif()
if (SDK_BUILD_OPENGLES_3_0)
	add_subdirectory(opengl_es_3.0)
endif()
