# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


add_executable(hello_world_c
	main.cpp
)

target_link_libraries(hello_world_c PRIVATE
	SdkCommon
)

install(TARGETS hello_world_c
	RUNTIME DESTINATION bin/hello_world_c
)
