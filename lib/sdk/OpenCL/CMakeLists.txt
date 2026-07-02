# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


find_package(OpenCL REQUIRED)

add_library(SdkOpenCL SHARED
	CLUtils.cpp
)

target_link_libraries(SdkOpenCL
	PRIVATE
		SdkCommon

	PUBLIC
		OpenCL::OpenCL
)
