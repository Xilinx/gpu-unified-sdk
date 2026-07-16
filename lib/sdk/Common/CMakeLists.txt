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

target_compile_definitions(SdkCommon PUBLIC
	LOG_ROOT_DIR=\"${CMAKE_CURRENT_LIST_DIR}\"
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
