# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(mandelbrot
	main.cpp
)

target_link_libraries(mandelbrot PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS mandelbrot
	RUNTIME DESTINATION bin/mandelbrot
)

install(DIRECTORY assets/
	DESTINATION bin/mandelbrot/assets
	FILES_MATCHING PATTERN "*"
)
