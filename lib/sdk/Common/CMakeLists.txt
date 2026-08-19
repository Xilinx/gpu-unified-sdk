# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


find_package(DRM REQUIRED)
find_package(PNG REQUIRED)

add_library(SdkCommon SHARED
	ArgumentParser.cpp
	Color.cpp
	FileUtils.cpp
	Folders.cpp
	FrameCounter.cpp
	GBMUtils.cpp
	Image.cpp
	# ImguiUtils.cpp
	Log.cpp
	Parser.cpp
	Pipe.cpp
	PngUtils.cpp
	SyncCallDebugger.cpp
	TextReader.cpp
	Utils.cpp
	# V4l2Utils.cpp
)

# Log lines print __FILE__ with this directory stripped off (see Log::context).
# Project-relative on purpose: the definition is PUBLIC, so an absolute path would be
# baked into the .rodata of every consumer, shipping the build machine's directory
# layout inside the binaries (Yocto rejects that with its "buildpaths" QA check).
file(RELATIVE_PATH LogRootDir "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_DIR}")

target_compile_definitions(SdkCommon PUBLIC
	LOG_ROOT_DIR=\"${LogRootDir}\"
)

target_link_libraries(SdkCommon
	PUBLIC
		SdkIf
		gbm
		drm

	PRIVATE
		# imgui
		png
)
