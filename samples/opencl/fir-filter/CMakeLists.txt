# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


add_executable(fir_filter
	main.cpp
)

target_link_libraries(fir_filter PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS fir_filter
	RUNTIME DESTINATION bin/fir_filter
)

install(DIRECTORY assets/
	DESTINATION bin/fir_filter/assets
	FILES_MATCHING PATTERN "*"
)
