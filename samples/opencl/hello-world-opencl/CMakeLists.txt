# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(hello_world_opencl
	main.cpp
)

target_link_libraries(hello_world_opencl PRIVATE
	SdkCommon
	SdkOpenCL
)

# Stage the binary and its assets in the shared output folder.
get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(_outdir "${SDK_OUTPUT_DIR}/opencl/${_sample_name}")
set_target_properties(hello_world_opencl PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")
add_custom_command(TARGET hello_world_opencl POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/assets"
		"${_outdir}/assets")

install(TARGETS hello_world_opencl
	RUNTIME DESTINATION bin/hello_world_opencl
)

install(DIRECTORY assets/
	DESTINATION bin/hello_world_opencl/assets
	FILES_MATCHING PATTERN "*"
)
