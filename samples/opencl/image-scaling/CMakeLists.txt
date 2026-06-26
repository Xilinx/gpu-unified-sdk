# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


add_executable(image_scaling
	main.cpp
)

target_link_libraries(image_scaling PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS image_scaling
	RUNTIME DESTINATION bin/image_scaling
)

install(DIRECTORY assets/
	DESTINATION bin/image_scaling/assets
	FILES_MATCHING PATTERN "*"
)
