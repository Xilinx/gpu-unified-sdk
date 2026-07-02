# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# particlerenderingsample - GPU particle / molecule system (OpenGL ES 2.0)
file(GLOB PARTICLERENDERINGSAMPLE_SOURCES
	"*.cpp"
)

add_executable(particlerenderingsample ${PARTICLERENDERINGSAMPLE_SOURCES})

target_link_libraries(particlerenderingsample PRIVATE
	SdkGlesFramework
)

target_include_directories(particlerenderingsample PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(particlerenderingsample PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(particlerenderingsample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET particlerenderingsample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")
add_custom_command(TARGET particlerenderingsample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${GLES_FONT_SHADERS_DIR}"
		"${_outdir}/FontShaders")

install(TARGETS particlerenderingsample
	RUNTIME DESTINATION bin/particlerenderingsample)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/particlerenderingsample)
install(DIRECTORY "${GLES_FONT_SHADERS_DIR}"
	DESTINATION bin/particlerenderingsample)
