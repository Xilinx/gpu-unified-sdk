# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# Make installed sample binaries find the SDK shared libraries (installed into
# "lib", see lib/sdk/CMakeLists.txt) via an $ORIGIN-relative RPATH, so they run
# without LD_LIBRARY_PATH. Two entries cover both supported install layouts:
#   bin/<sample>/<binary>      + lib/   -> $ORIGIN/../../lib
#   <api>/<sample>/<binary>    + <api>/lib/ (packaged layout) -> $ORIGIN/../lib
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN/../../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# --- Shared SDK library directory for the build output tree ------------------
# All samples that link the SDK shared libraries (OpenCL and Vulkan; the OpenGL
# ES samples link the STATIC framework and need none) resolve them from a single
# shared directory "${SDK_OUTPUT_DIR}/lib". Every such sample sits two levels
# below SDK_OUTPUT_DIR:
#   ${SDK_OUTPUT_DIR}/opencl/<sample>/<binary>
#   ${SDK_OUTPUT_DIR}/vulkan/<sample>/<binary>
# so a single build RPATH of "$ORIGIN/../../lib" works for all of them and no
# per-app LD_LIBRARY_PATH is required. The SDK_SHARED_LIB_DIR / SDK_SHARED_LIB_RPATH
# variables below are consumed by samples/opencl and samples/vulkan.
set(SDK_SHARED_LIB_DIR "${SDK_OUTPUT_DIR}/lib" CACHE INTERNAL
	"Directory the SDK shared libraries are staged into for the output tree")
set(SDK_SHARED_LIB_RPATH "$ORIGIN/../../lib" CACHE INTERNAL
	"Common $ORIGIN-relative RPATH pointing samples at SDK_SHARED_LIB_DIR")

# Stage the SDK shared libraries into the single shared lib dir. Only the
# libraries that exist for the configured APIs are staged.
set(_sdk_shared_lib_targets)
set(_sdk_shared_lib_files)
foreach(_lib SdkCommon SdkOpenCL SdkVulkan)
	if (TARGET ${_lib})
		get_target_property(_lib_type ${_lib} TYPE)
		if (_lib_type STREQUAL "SHARED_LIBRARY")
			list(APPEND _sdk_shared_lib_targets ${_lib})
			list(APPEND _sdk_shared_lib_files $<TARGET_FILE:${_lib}>)
		endif()
	endif()
endforeach()
if (_sdk_shared_lib_files)
	add_custom_target(sdk_shared_libs ALL
		COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_SHARED_LIB_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${_sdk_shared_lib_files} "${SDK_SHARED_LIB_DIR}"
		DEPENDS ${_sdk_shared_lib_targets}
		COMMENT "Staging SDK shared libraries into ${SDK_SHARED_LIB_DIR}")
endif()

# Each sample set is gated by its per-API switch (see the top-level CMakeLists
# for SDK_PLATFORM_TARGET and the SDK_BUILD_<API> options).
if (SDK_BUILD_OPENCL)
	add_subdirectory(opencl)
endif()
if (SDK_BUILD_VULKAN)
	add_subdirectory(vulkan)
endif()
if (SDK_BUILD_OPENGLES)
	add_subdirectory(opengles)
endif()
