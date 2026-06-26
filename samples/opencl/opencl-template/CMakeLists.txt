# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


add_executable(opencl_template
	main.cpp
)

target_link_libraries(opencl_template PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS opencl_template
	RUNTIME DESTINATION bin/opencl_template
)

install(DIRECTORY assets/
	DESTINATION bin/opencl_template/assets
	FILES_MATCHING PATTERN "*"
)
