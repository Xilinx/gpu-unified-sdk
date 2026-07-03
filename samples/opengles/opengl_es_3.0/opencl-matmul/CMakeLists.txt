# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# opencl-matmul - OpenCL matrix multiplication on the GPU.#
# This sample is pure OpenCL (no EGL/GLES windowing), so it does not link the
# GLES framework. It is grouped under opengl_es_3.0 for layout consistency.

find_package(OpenCL QUIET)
if(NOT OpenCL_FOUND)
	message(STATUS "opencl-matmul: OpenCL not found, skipping target")
	return()
endif()

file(GLOB OPENCL_MATMUL_SOURCES
	"*.cpp"
)

add_executable(opencl-matmul ${OPENCL_MATMUL_SOURCES})

target_link_libraries(opencl-matmul PRIVATE
	OpenCL::OpenCL
)

set_target_properties(opencl-matmul PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
get_filename_component(_gles_version "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(_gles_version "${_gles_version}" NAME)
string(TOLOWER "${SDK_Platform}" _backend_name)
set(_outdir "${SDK_OUTPUT_DIR}/opengles/${_backend_name}/${_gles_version}/${_sample_name}")
set_target_properties(opencl-matmul PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")

add_custom_command(TARGET opencl-matmul POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/cl_kernels"
		"${_outdir}/cl_kernels")

install(TARGETS opencl-matmul
	RUNTIME DESTINATION bin/opencl-matmul)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cl_kernels"
	DESTINATION bin/opencl-matmul)
