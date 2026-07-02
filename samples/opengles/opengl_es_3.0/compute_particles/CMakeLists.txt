# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# compute_particles - compute-shader particle system (OpenGL ES 3.1)
file(GLOB COMPUTE_PARTICLES_SOURCES
	"*.cpp"
	"common/*.cpp"
)

add_executable(compute_particles ${COMPUTE_PARTICLES_SOURCES})

target_link_libraries(compute_particles PRIVATE
	SdkGlesFramework
)

target_include_directories(compute_particles PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(compute_particles PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET compute_particles POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")

install(TARGETS compute_particles
	RUNTIME DESTINATION bin/compute_particles)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/compute_particles)
