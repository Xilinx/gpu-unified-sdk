# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(sobel_filter_no_vectors
	main.cpp
)

target_link_libraries(sobel_filter_no_vectors PRIVATE
	SdkCommon
	SdkOpenCL
)

# Stage the binary and its assets in the shared output folder.
get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(_outdir "${SDK_OUTPUT_DIR}/opencl/${_sample_name}")
set_target_properties(sobel_filter_no_vectors PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")
add_custom_command(TARGET sobel_filter_no_vectors POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/assets"
		"${_outdir}/assets")

install(TARGETS sobel_filter_no_vectors
	RUNTIME DESTINATION bin/sobel_filter_no_vectors
)

install(DIRECTORY assets/
	DESTINATION bin/sobel_filter_no_vectors/assets
	FILES_MATCHING PATTERN "*"
)
