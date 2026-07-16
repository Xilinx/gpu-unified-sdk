# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


# The Vulkan samples link the SDK shared libraries (libSdkVulkan/libSdkCommon),
# which are staged into the single shared "${SDK_OUTPUT_DIR}/lib" directory (see
# samples/CMakeLists.txt). Give every sample in this subtree the common build
# RPATH so they load those libraries without LD_LIBRARY_PATH.
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CMAKE_BUILD_RPATH "${SDK_SHARED_LIB_RPATH}")

add_subdirectory(basic-compute)
add_subdirectory(hellotriangle)
add_subdirectory(rotating-texture)
add_subdirectory(spinning-cube)
add_subdirectory(multipass)
add_subdirectory(multisampling)
add_subdirectory(mipmapping)
add_subdirectory(astc)
add_subdirectory(multithreading)
