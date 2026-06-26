# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


# SdkGlesFramework
#
# The OpenGL ES core framework (EGL setup, windowing podium, GLSL compile,
# matrix API, font engine, texture loaders) that the GLES samples are built on.
#
# Windowing backend selection (exposes ENABLE_WAYLAND / ENABLE_X11 /
# ENABLE_FBDEV):
#
#     -D SDK_Platform=wayland   (default) Wayland + xdg-shell  -> ENABLE_WAYLAND
#     -D SDK_Platform=x11                 X11 windowing         -> ENABLE_X11
#     -D SDK_Platform=fbdev               Linux framebuffer     -> ENABLE_FBDEV
#
# Exactly one backend is compiled; only that backend's podium source and its
# system libraries are pulled in. The xdg-shell client protocol is generated
# with wayland-scanner at build time (Wayland backend only).

set(SDK_Platform "wayland" CACHE STRING
	"OpenGL ES windowing backend: wayland | x11 | fbdev")
set_property(CACHE SDK_Platform PROPERTY STRINGS wayland x11 fbdev)

string(TOLOWER "${SDK_Platform}" _gles_platform)
if (NOT _gles_platform MATCHES "^(wayland|x11|fbdev)$")
	message(FATAL_ERROR
		"SDK_Platform must be one of: wayland, x11, fbdev (got '${SDK_Platform}')")
endif()
message(STATUS "SdkGlesFramework: windowing backend = ${_gles_platform}")

# Sources common to every backend.
set(GLES_FRAMEWORK_SOURCES
	XEGLIntf.cpp
	XPodium.cpp
	XGLSLCompile.cpp
	XMatrixAPI.cpp
	XTime.cpp
	FontEngine.cpp
	XTexture.cpp
	XCompressionTexture.cpp
)

# EGL + GLESv2 are required by all backends.
find_package(PkgConfig REQUIRED)
pkg_check_modules(EGL   REQUIRED egl)
pkg_check_modules(GLES2 REQUIRED glesv2)

# Sysroot helper: the Yocto pkg-config wrapper can return include/data dirs with
# the sysroot prefixed twice; collapse a doubled prefix so paths resolve.
if (DEFINED CMAKE_SYSROOT AND CMAKE_SYSROOT)
	set(_sysroot "${CMAKE_SYSROOT}")
elseif (CMAKE_FIND_ROOT_PATH)
	list(GET CMAKE_FIND_ROOT_PATH 0 _sysroot)
endif()

set(_backend_define)
set(_backend_include_dirs)
set(_backend_libraries)
set(_backend_library_dirs)

if (_gles_platform STREQUAL "wayland")
	set(_backend_define ENABLE_WAYLAND)
	list(APPEND GLES_FRAMEWORK_SOURCES XWaylandPodium.cpp)

	pkg_check_modules(WAYLAND_CLIENT    REQUIRED wayland-client)
	pkg_check_modules(WAYLAND_EGL       REQUIRED wayland-egl)
	pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

	# Generate the xdg-shell client protocol (header + private code).
	pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
	find_program(WAYLAND_SCANNER wayland-scanner)
	if (NOT WAYLAND_SCANNER)
		message(FATAL_ERROR "wayland-scanner not found (needed to generate xdg-shell protocol)")
	endif()
	if (_sysroot)
		string(REPLACE "${_sysroot}${_sysroot}" "${_sysroot}"
			WAYLAND_PROTOCOLS_DIR "${WAYLAND_PROTOCOLS_DIR}")
	endif()

	set(XDG_SHELL_PROTOCOL "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml")
	if (NOT EXISTS "${XDG_SHELL_PROTOCOL}")
		message(FATAL_ERROR
			"xdg-shell.xml not found at:\n  ${XDG_SHELL_PROTOCOL}\n"
			"(resolved wayland-protocols pkgdatadir = ${WAYLAND_PROTOCOLS_DIR})")
	endif()
	set(XDG_SHELL_HEADER "${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.h")
	set(XDG_SHELL_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c")

	add_custom_command(
		OUTPUT "${XDG_SHELL_HEADER}"
		COMMAND ${WAYLAND_SCANNER} client-header "${XDG_SHELL_PROTOCOL}" "${XDG_SHELL_HEADER}"
		DEPENDS "${XDG_SHELL_PROTOCOL}"
		COMMENT "Generating XDG shell client protocol header"
		VERBATIM
	)
	add_custom_command(
		OUTPUT "${XDG_SHELL_SOURCE}"
		COMMAND ${WAYLAND_SCANNER} private-code "${XDG_SHELL_PROTOCOL}" "${XDG_SHELL_SOURCE}"
		DEPENDS "${XDG_SHELL_PROTOCOL}"
		COMMENT "Generating XDG shell protocol source"
		VERBATIM
	)
	list(APPEND GLES_FRAMEWORK_SOURCES "${XDG_SHELL_SOURCE}" "${XDG_SHELL_HEADER}")

	set(_backend_include_dirs
		${WAYLAND_CLIENT_INCLUDE_DIRS} ${WAYLAND_EGL_INCLUDE_DIRS})
	set(_backend_libraries
		${WAYLAND_CLIENT_LIBRARIES} ${WAYLAND_EGL_LIBRARIES})
	set(_backend_library_dirs
		${WAYLAND_CLIENT_LIBRARY_DIRS} ${WAYLAND_EGL_LIBRARY_DIRS})

elseif (_gles_platform STREQUAL "x11")
	set(_backend_define ENABLE_X11)
	list(APPEND GLES_FRAMEWORK_SOURCES XLinuxPodium.cpp)

	find_package(X11 REQUIRED)
	set(_backend_include_dirs ${X11_INCLUDE_DIR})
	set(_backend_libraries    ${X11_LIBRARIES})

else() # fbdev
	set(_backend_define ENABLE_FBDEV)
	list(APPEND GLES_FRAMEWORK_SOURCES XLinuxTarget.cpp)
	# fbdev uses EGL_DEFAULT_DISPLAY and a plain fbdev_window struct: no extra
	# system libraries beyond EGL/GLESv2.
endif()

add_library(SdkGlesFramework STATIC ${GLES_FRAMEWORK_SOURCES})

# Select the windowing backend at compile time. PUBLIC so the samples (which
# include XPodium.h) see the same backend macro and pull in the matching podium.
target_compile_definitions(SdkGlesFramework PUBLIC ${_backend_define})

# Expose the framework headers to consumers (samples include "XPodium.h" etc.).
# The generated xdg-shell header (Wayland) lives in the binary dir and must be
# PUBLIC because XWaylandPodium.h (pulled in transitively via XPodium.h) needs it.
target_include_directories(SdkGlesFramework
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_BINARY_DIR}"
)

# Backend + EGL/GLES include dirs from the Yocto pkg-config wrapper can come back
# with the sysroot prefixed twice; add them only when they actually exist so a
# bogus doubled path is never used.
foreach (_inc ${_backend_include_dirs} ${EGL_INCLUDE_DIRS} ${GLES2_INCLUDE_DIRS})
	if (_sysroot)
		string(REPLACE "${_sysroot}${_sysroot}" "${_sysroot}" _inc "${_inc}")
	endif()
	if (EXISTS "${_inc}")
		target_include_directories(SdkGlesFramework PUBLIC "${_inc}")
	endif()
endforeach()

target_link_libraries(SdkGlesFramework
	PUBLIC
		${_backend_libraries}
		${EGL_LIBRARIES}
		${GLES2_LIBRARIES}
)

target_link_directories(SdkGlesFramework PUBLIC
	${_backend_library_dirs}
	${EGL_LIBRARY_DIRS}
	${GLES2_LIBRARY_DIRS}
)

# Publish the selected backend to the parent scope so the samples tree can make
# backend-specific decisions (e.g. skip Wayland-incompatible samples).
set(SDK_GLES_PLATFORM "${_gles_platform}" CACHE INTERNAL
	"Resolved OpenGL ES windowing backend")
