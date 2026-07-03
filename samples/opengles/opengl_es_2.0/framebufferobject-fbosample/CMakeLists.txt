# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# framebufferobject-fbosample - render-to-texture via FBO (OpenGL ES 2.0)
file(GLOB FRAMEBUFFEROBJECT-FBOSAMPLE_SOURCES
	"*.cpp"
)

add_executable(framebufferobject-fbosample ${FRAMEBUFFEROBJECT-FBOSAMPLE_SOURCES})

target_link_libraries(framebufferobject-fbosample PRIVATE
	SdkGlesFramework
)

target_include_directories(framebufferobject-fbosample PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(framebufferobject-fbosample PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
get_filename_component(_gles_version "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(_gles_version "${_gles_version}" NAME)
string(TOLOWER "${SDK_Platform}" _backend_name)
set(_outdir "${SDK_OUTPUT_DIR}/opengles/${_backend_name}/${_gles_version}/${_sample_name}")
set_target_properties(framebufferobject-fbosample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET framebufferobject-fbosample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")
add_custom_command(TARGET framebufferobject-fbosample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${GLES_FONT_SHADERS_DIR}"
		"${_outdir}/FontShaders")

install(TARGETS framebufferobject-fbosample
	RUNTIME DESTINATION bin/framebufferobject-fbosample)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/framebufferobject-fbosample)
install(DIRECTORY "${GLES_FONT_SHADERS_DIR}"
	DESTINATION bin/framebufferobject-fbosample)
