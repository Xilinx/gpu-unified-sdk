# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(obstacle_detection
	main.cpp
)

target_link_libraries(obstacle_detection PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS obstacle_detection
	RUNTIME DESTINATION bin/obstacle_detection
)

install(DIRECTORY assets/
	DESTINATION bin/obstacle_detection/assets
	FILES_MATCHING PATTERN "*"
)
