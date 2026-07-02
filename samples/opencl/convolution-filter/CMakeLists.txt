# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(convolution_filter
	main.cpp
)

target_link_libraries(convolution_filter PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS convolution_filter
	RUNTIME DESTINATION bin/convolution_filter
)

install(DIRECTORY assets/
	DESTINATION bin/convolution_filter/assets
	FILES_MATCHING PATTERN "*"
)
