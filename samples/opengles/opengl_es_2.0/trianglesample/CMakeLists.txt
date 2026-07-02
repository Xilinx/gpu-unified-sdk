# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# trianglesample - anti-aliased spinning triangle (OpenGL ES 2.0).

add_executable(trianglesample
	TriangleSample.cpp
)

target_link_libraries(trianglesample PRIVATE
	SdkGlesFramework
)

target_compile_definitions(trianglesample PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Place the binary and its staged assets in a dedicated output directory so the
# asset copies below never target the source tree (the SDK uses in-source-style
# output dirs).
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(trianglesample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

# Stage the sample's shaders (Supportingfiles/) and the shared FontShaders/
# folder next to the executable.
add_custom_command(TARGET trianglesample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")
add_custom_command(TARGET trianglesample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${GLES_FONT_SHADERS_DIR}"
		"${_outdir}/FontShaders")

install(TARGETS trianglesample
	RUNTIME DESTINATION bin/trianglesample)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/trianglesample)
install(DIRECTORY "${GLES_FONT_SHADERS_DIR}"
	DESTINATION bin/trianglesample)
