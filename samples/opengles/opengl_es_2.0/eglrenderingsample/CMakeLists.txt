# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# eglrenderingsample - EGLImage rendering (OpenGL ES 2.0).#
# This sample creates its own EGL surface directly instead of using the
# SdkGlesFramework podium, so it is built standalone against X11 + EGL + GLESv2 and
# does NOT link SdkGlesFramework. Its surface covers the whole framebuffer, so it
# sizes itself from the panel and takes the shared window-size defines only when one
# was explicitly configured (see below).
#
# It drives the Mali fbdev path only: it opens /dev/fb0, maps /dev/mem and passes
# EGL_DEFAULT_DISPLAY to eglGetDisplay, so on x11 or wayland it would build but
# never run. The marker below is what the Yocto recipe reads to keep this target
# out of its per-backend build list; keep the two in step.
# SDK_BACKENDS: fbdev
string(TOLOWER "${SDK_Platform}" _backend_name)
if (NOT _backend_name STREQUAL "fbdev")
	message(STATUS "eglrenderingsample: SDK_Platform=${SDK_Platform}, skipping (fbdev only)")
	return()
endif()

find_package(X11 QUIET)
if(NOT X11_FOUND)
	message(STATUS "eglrenderingsample: X11 not found, skipping target")
	return()
endif()

file(GLOB EGLRENDERINGSAMPLE_SOURCES
	"*.c"
)

add_executable(eglrenderingsample ${EGLRENDERINGSAMPLE_SOURCES})

target_link_libraries(eglrenderingsample PRIVATE
	EGL
	GLESv2
	${X11_LIBRARIES}
	Xext
	pthread
	dl
	m
)

target_include_directories(eglrenderingsample PRIVATE
	${X11_INCLUDE_DIR}
)

# Without these the sample renders across the whole framebuffer, which is what its
# surface always covers; they are passed only when a window size was explicitly
# configured, and then it renders into that rectangle instead.
if (GLES_WINDOW_SIZE_EXPLICIT)
	target_compile_definitions(eglrenderingsample PRIVATE
		WINDOW_W=${GLES_WINDOW_W}
		WINDOW_H=${GLES_WINDOW_H}
	)
endif()

get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
get_filename_component(_gles_version "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(_gles_version "${_gles_version}" NAME)
set(_outdir "${SDK_OUTPUT_DIR}/opengles/${_backend_name}/${_gles_version}/${_sample_name}")
set_target_properties(eglrenderingsample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

install(TARGETS eglrenderingsample
	RUNTIME DESTINATION bin/eglrenderingsample)
