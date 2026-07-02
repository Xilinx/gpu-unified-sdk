# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


cmake_minimum_required(VERSION 3.20)

project(SDK)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This CMakeLists sits at the root of the SDK tree, so SdkDir IS the SDK root and
# also the project root (the 3rdparty/ and cmake/ folders live directly under it).
get_filename_component(SdkDir "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
set(RootDir "${SdkDir}")
set(ThirdPartyDir "${SdkDir}/3rdparty")
set(ModulesDir "${SdkDir}/cmake/modules")

# ---------------------------------------------------------------------------
# Build subset selection
# ---------------------------------------------------------------------------
# Nothing is built by default: you must opt in to the sample sets you want,
# either via a platform preset or via the explicit per-API switches.
#
# Two complementary controls decide which sample sets are built:
#
#   1. SDK_PLATFORM_TARGET - optional target SoC preset that sets sensible
#      DEFAULTS based on that platform's GPU capabilities:
#        none (default)    - nothing enabled (use the per-API switches)
#        versal-2ve-2vm    - Mali G78AE: OpenGL ES 2.0 + 3.0, OpenCL, Vulkan
#        zynqmp            - Mali 400  : OpenGL ES 2.0 only
#
#   2. SDK_BUILD_<API> / SDK_BUILD_OPENGLES_<ver> - explicit per-API and per-
#      GLES-version ON/OFF switches. If the user sets one of these on the command
#      line it OVERRIDES the platform-derived default, so any subset can be built
#      regardless of platform, e.g.:
#        -D SDK_BUILD_OPENCL=ON   -> build only the OpenCL samples
#
set(SDK_PLATFORM_TARGET "none" CACHE STRING
	"Target SoC preset for default sample selection: none | versal-2ve-2vm | zynqmp")
set_property(CACHE SDK_PLATFORM_TARGET PROPERTY STRINGS none versal-2ve-2vm zynqmp)

if (SDK_PLATFORM_TARGET STREQUAL "none")
	# No preset: everything OFF by default; enable via SDK_BUILD_* switches.
	set(_def_opengles OFF)
	set(_def_gles2    OFF)
	set(_def_gles3    OFF)
	set(_def_opencl   OFF)
	set(_def_vulkan   OFF)
elseif (SDK_PLATFORM_TARGET STREQUAL "zynqmp")
	# Mali 400: OpenGL ES 2.0 only.
	set(_def_opengles ON)
	set(_def_gles2    ON)
	set(_def_gles3    OFF)
	set(_def_opencl   OFF)
	set(_def_vulkan   OFF)
elseif (SDK_PLATFORM_TARGET STREQUAL "versal-2ve-2vm")
	# Mali G78AE: everything.
	set(_def_opengles ON)
	set(_def_gles2    ON)
	set(_def_gles3    ON)
	set(_def_opencl   ON)
	set(_def_vulkan   ON)
else()
	message(FATAL_ERROR
		"SDK_PLATFORM_TARGET must be 'none', 'versal-2ve-2vm' or 'zynqmp' (got '${SDK_PLATFORM_TARGET}')")
endif()

# Per-API switches (defaults come from the selected platform; an explicit
# -D on the command line overrides the default).
option(SDK_BUILD_OPENGLES "Build the OpenGL ES samples" ${_def_opengles})
option(SDK_BUILD_OPENCL   "Build the OpenCL samples"    ${_def_opencl})
option(SDK_BUILD_VULKAN   "Build the Vulkan samples"    ${_def_vulkan})

# OpenGL ES version sub-selection (only meaningful when SDK_BUILD_OPENGLES is ON).
option(SDK_BUILD_OPENGLES_2_0 "Build the OpenGL ES 2.0 samples" ${_def_gles2})
option(SDK_BUILD_OPENGLES_3_0 "Build the OpenGL ES 3.0 samples" ${_def_gles3})

# Convenience: if OpenGL ES is enabled (e.g. via -D SDK_BUILD_OPENGLES=ON with no
# platform preset) but neither version was explicitly selected, build both so the
# user is not left with an empty OpenGL ES build.
if (SDK_BUILD_OPENGLES AND NOT SDK_BUILD_OPENGLES_2_0 AND NOT SDK_BUILD_OPENGLES_3_0)
	message(STATUS "SDK_BUILD_OPENGLES is ON with no version selected; enabling both 2.0 and 3.0")
	set(SDK_BUILD_OPENGLES_2_0 ON)
	set(SDK_BUILD_OPENGLES_3_0 ON)
endif()

message(STATUS "GPU Unified SDK build selection (platform: ${SDK_PLATFORM_TARGET}):")
message(STATUS "  OpenGL ES : ${SDK_BUILD_OPENGLES} (2.0=${SDK_BUILD_OPENGLES_2_0}, 3.0=${SDK_BUILD_OPENGLES_3_0})")
message(STATUS "  OpenCL    : ${SDK_BUILD_OPENCL}")
message(STATUS "  Vulkan    : ${SDK_BUILD_VULKAN}")

add_subdirectory(lib/sdk)
add_subdirectory(samples)
