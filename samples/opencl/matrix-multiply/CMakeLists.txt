# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(matrix_multiply
	main.cpp
)

target_link_libraries(matrix_multiply PRIVATE
	SdkCommon
	SdkOpenCL
)

# Stage the binary and its assets in the shared output folder.
get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(_outdir "${SDK_OUTPUT_DIR}/opencl/${_sample_name}")
set_target_properties(matrix_multiply PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")
add_custom_command(TARGET matrix_multiply POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/assets"
		"${_outdir}/assets")

install(TARGETS matrix_multiply
	RUNTIME DESTINATION bin/matrix_multiply
)

install(DIRECTORY assets/
	DESTINATION bin/matrix_multiply/assets
	FILES_MATCHING PATTERN "*"
)
