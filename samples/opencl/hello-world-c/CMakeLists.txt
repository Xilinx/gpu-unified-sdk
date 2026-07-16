# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(hello_world_c
	main.cpp
)

target_link_libraries(hello_world_c PRIVATE
	SdkCommon
)

# Stage the binary in the shared output folder (this sample has no assets).
get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(_outdir "${SDK_OUTPUT_DIR}/opencl/${_sample_name}")
set_target_properties(hello_world_c PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")

install(TARGETS hello_world_c
	RUNTIME DESTINATION bin/hello_world_c
)
