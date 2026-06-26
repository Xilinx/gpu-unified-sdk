# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


add_executable(sgemm
	main.cpp
)

target_link_libraries(sgemm PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS sgemm
	RUNTIME DESTINATION bin/sgemm
)

install(DIRECTORY assets/
	DESTINATION bin/sgemm/assets
	FILES_MATCHING PATTERN "*"
)
