# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(mandelbrot_bench
	main.cpp
)

target_link_libraries(mandelbrot_bench PRIVATE
	SdkCommon
	SdkOpenCL
)

install(TARGETS mandelbrot_bench
	RUNTIME DESTINATION bin/mandelbrot_bench
)

install(DIRECTORY assets/
	DESTINATION bin/mandelbrot_bench/assets
	FILES_MATCHING PATTERN "*"
)
