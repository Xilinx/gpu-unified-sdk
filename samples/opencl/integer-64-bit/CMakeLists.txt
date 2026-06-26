# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


add_executable(integer_64_bit
	main.cpp
)

target_link_libraries(integer_64_bit PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS integer_64_bit
	RUNTIME DESTINATION bin/integer_64_bit
)

install(DIRECTORY assets/
	DESTINATION bin/integer_64_bit/assets
	FILES_MATCHING PATTERN "*"
)
