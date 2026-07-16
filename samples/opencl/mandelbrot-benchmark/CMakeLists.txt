# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(mandelbrot_bench
	main.cpp
)

target_link_libraries(mandelbrot_bench PRIVATE
	SdkCommon
	SdkOpenCL
)

# Stage the binary and its assets in the shared output folder.
get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(_outdir "${SDK_OUTPUT_DIR}/opencl/${_sample_name}")
set_target_properties(mandelbrot_bench PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")
add_custom_command(TARGET mandelbrot_bench POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/assets"
		"${_outdir}/assets")

install(TARGETS mandelbrot_bench
	RUNTIME DESTINATION bin/mandelbrot_bench
)

install(DIRECTORY assets/
	DESTINATION bin/mandelbrot_bench/assets
	FILES_MATCHING PATTERN "*"
)
