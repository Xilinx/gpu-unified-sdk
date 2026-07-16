# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# The OpenCL samples link the SDK shared libraries (libSdkOpenCL/libSdkCommon),
# which are staged into the single shared "${SDK_OUTPUT_DIR}/lib" directory (see
# samples/CMakeLists.txt). Give every sample in this subtree the common build
# RPATH so they load those libraries without LD_LIBRARY_PATH.
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CMAKE_BUILD_RPATH "${SDK_SHARED_LIB_RPATH}")

add_subdirectory(adas)
add_subdirectory(convolution-filter)
add_subdirectory(depth-map)
add_subdirectory(fir-filter)
add_subdirectory(gaussian-filter)
add_subdirectory(hello-world-c)
add_subdirectory(hello-world-opencl)
add_subdirectory(image-scaling)
add_subdirectory(integer-64-bit)
add_subdirectory(mandelbrot)
add_subdirectory(mandelbrot-benchmark)
add_subdirectory(matrix-multiply)
add_subdirectory(ml-classifier)
add_subdirectory(opencl-template)
add_subdirectory(sgemm)
add_subdirectory(sobel-filter)
add_subdirectory(sobel-filter-no-vectors)
add_subdirectory(vector-hello-world)
