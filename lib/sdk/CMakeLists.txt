# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


cmake_minimum_required(VERSION 3.22)

project(LibSdk)


set(ECM_DIR       "${ModulesDir}/ecm" CACHE STRING "" FORCE)
set(GStreamer_DIR "${ModulesDir}/gstreamer" CACHE STRING "" FORCE)
set(DRM_DIR       "${ModulesDir}/drm" CACHE STRING "" FORCE)


# common interface library
add_library(sdk_compile_flags INTERFACE)

target_compile_options(sdk_compile_flags INTERFACE
	-Werror
	-Wall
)

target_include_directories(sdk_compile_flags INTERFACE
	"${CMAKE_CURRENT_LIST_DIR}"
)

if (SDK_DEV)
	# skip warnings about unused vars in dev mode
	target_compile_options(sdk_compile_flags INTERFACE
		-Wno-error=unused-variable
		-Wno-error=unused-but-set-variable
	)
endif()

if (SDK_HOST)
	target_compile_definitions(sdk_compile_flags INTERFACE
		SDK_SHARE_DIR=\"${DemoDir}\"
	)
endif()


if (Sdk_UseDma)
	target_compile_definitions(sdk_compile_flags INTERFACE
		SDK_USE_DMA
	)
endif()


# common interface properties
add_library(SdkIf INTERFACE)

target_include_directories(SdkIf INTERFACE
	"${ThisDir}"
)


# SdkCommon is shared by every sample set, so it is always built. The
# API-specific support libraries are only built when their sample set is
# enabled, so a subset build (e.g. OpenGL ES only on ZynqMP) does not require
# the OpenCL / Vulkan / glm toolchain packages.
add_subdirectory(Common)
if (SDK_BUILD_OPENCL)
	add_subdirectory(OpenCL)
endif()
if (SDK_BUILD_VULKAN)
	add_subdirectory(Vulkan)
endif()
if (SDK_BUILD_OPENGLES)
	add_subdirectory(GlesFramework)
endif()

# Install the SDK shared libraries so the OpenCL/Vulkan samples can load them on
# the target. They are installed into "lib" (relative to CMAKE_INSTALL_PREFIX);
# the samples set an $ORIGIN-relative RPATH (see samples/CMakeLists.txt) so each
# bin/<sample>/<binary> finds these libraries without LD_LIBRARY_PATH.
#
# Only install the shared SDK libraries that were actually added above (SdkOpenGL
# is not built in the default configuration).
set(_sdk_install_libs)
foreach(_lib SdkCommon SdkOpenCL SdkVulkan SdkOpenGL)
	if (TARGET ${_lib})
		get_target_property(_lib_type ${_lib} TYPE)
		if (_lib_type STREQUAL "SHARED_LIBRARY")
			list(APPEND _sdk_install_libs ${_lib})
			# Give each SDK shared library an $ORIGIN RPATH so it can locate its
			# sibling SDK libraries (all installed side-by-side in "lib/"). This
			# is required for TRANSITIVE loads: e.g. a sample's RUNPATH lets the
			# loader find libSdkVulkan.so, but RUNPATH (unlike RPATH) is NOT used
			# to resolve that library's own dependency on libSdkCommon.so. Baking
			# $ORIGIN into the libraries themselves makes libSdkVulkan.so /
			# libSdkOpenCL.so find libSdkCommon.so regardless of how they were
			# loaded. INSTALL_RPATH is applied during install (CMake's install-time
			# RPATH edit, which the Yocto recipe triggers via cmake_install.cmake),
			# so build-tree runs keep their normal link-path RPATH.
			set_target_properties(${_lib} PROPERTIES
				INSTALL_RPATH "$ORIGIN")
		endif()
	endif()
endforeach()
if (_sdk_install_libs)
	install(TARGETS ${_sdk_install_libs}
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION lib)
endif()
