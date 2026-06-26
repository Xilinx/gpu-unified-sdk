# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


# Make installed sample binaries find the SDK shared libraries (installed into
# "lib", see lib/sdk/CMakeLists.txt) via an $ORIGIN-relative RPATH, so they run
# without LD_LIBRARY_PATH. Two entries cover both supported install layouts:
#   bin/<sample>/<binary>      + lib/   -> $ORIGIN/../../lib
#   <api>/<sample>/<binary>    + <api>/lib/ (packaged layout) -> $ORIGIN/../lib
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN/../../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# Each sample set is gated by its per-API switch (see the top-level CMakeLists
# for SDK_PLATFORM_TARGET and the SDK_BUILD_<API> options).
if (SDK_BUILD_OPENCL)
	add_subdirectory(opencl)
endif()
if (SDK_BUILD_VULKAN)
	add_subdirectory(vulkan)
endif()
if (SDK_BUILD_OPENGLES)
	add_subdirectory(opengles)
endif()
