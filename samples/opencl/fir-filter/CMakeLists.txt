# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(fir_filter
	main.cpp
)

target_link_libraries(fir_filter PRIVATE
	SdkCommon
	SdkOpenCL
)

# Stage the binary and its assets in the shared output folder.
get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(_outdir "${SDK_OUTPUT_DIR}/opencl/${_sample_name}")
set_target_properties(fir_filter PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")
add_custom_command(TARGET fir_filter POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/assets"
		"${_outdir}/assets")

install(TARGETS fir_filter
	RUNTIME DESTINATION bin/fir_filter
)

install(DIRECTORY assets/
	DESTINATION bin/fir_filter/assets
	FILES_MATCHING PATTERN "*"
)
