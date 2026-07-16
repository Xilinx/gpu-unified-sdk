# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


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

# Stage the backend's runtime-environment "exports" helper next to the samples so
# it can be sourced before launching one (mirrors the Yocto recipe, which ships
# the same script next to the installed samples). It sets the backend-specific
# runtime vars for the configured SDK_Platform and selects the matching Mali
# driver variant. The script lives one level above the version dirs so it applies
# to both opengl_es_2.0 and _3.0:
#   ${SDK_OUTPUT_DIR}/opengles/<backend>/run_exports_<backend>.sh
# The X11 script is named with an uppercase "X11" in the source tree.
string(TOLOWER "${SDK_Platform}" _gles_backend)
if (_gles_backend STREQUAL "x11")
	set(_gles_exports_script "run_exports_X11.sh")
else()
	set(_gles_exports_script "run_exports_${_gles_backend}.sh")
endif()
set(_gles_exports_src "${CMAKE_CURRENT_LIST_DIR}/${_gles_exports_script}")
if (EXISTS "${_gles_exports_src}")
	set(_gles_exports_dst
		"${SDK_OUTPUT_DIR}/opengles/${_gles_backend}/${_gles_exports_script}")
	add_custom_target(gles_run_exports ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${_gles_exports_src}" "${_gles_exports_dst}"
		BYPRODUCTS "${_gles_exports_dst}"
		COMMENT "Staging OpenGL ES runtime exports (${_gles_exports_script})"
		VERBATIM)
else()
	message(WARNING
		"OpenGL ES runtime-exports script not found: ${_gles_exports_src}")
endif()

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
