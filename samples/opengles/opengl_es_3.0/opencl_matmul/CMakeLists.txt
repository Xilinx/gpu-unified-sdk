# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# opencl_matmul - OpenCL matrix multiplication on the GPU.#
# This sample is pure OpenCL (no EGL/GLES windowing), so it does not link the
# GLES framework. It is grouped under opengl_es_3.0 for layout consistency.

find_package(OpenCL QUIET)
if(NOT OpenCL_FOUND)
	message(STATUS "opencl_matmul: OpenCL not found, skipping target")
	return()
endif()

file(GLOB OPENCL_MATMUL_SOURCES
	"*.cpp"
)

add_executable(opencl_matmul ${OPENCL_MATMUL_SOURCES})

target_link_libraries(opencl_matmul PRIVATE
	OpenCL::OpenCL
)

set_target_properties(opencl_matmul PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(opencl_matmul PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")

add_custom_command(TARGET opencl_matmul POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/cl_kernels"
		"${_outdir}/cl_kernels")

install(TARGETS opencl_matmul
	RUNTIME DESTINATION bin/opencl_matmul)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cl_kernels"
	DESTINATION bin/opencl_matmul)
