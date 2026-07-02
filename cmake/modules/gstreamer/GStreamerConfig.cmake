# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


find_package(GTK2 REQUIRED gtk)

find_path(GstLocation gst/gst.h PATH_SUFFIXES gstreamer-1.0)
if (NOT GstLocation)
	message(FATAL_ERROR "gstreamer-1.0 headers not found")
endif()

add_library(gstreamer INTERFACE)

target_include_directories(gstreamer INTERFACE "${GstLocation}")

target_link_libraries(gstreamer INTERFACE
	GTK2::glib
	GTK2::gobject
	gstreamer-1.0
	gstallocators-1.0
	gstvideo-1.0
)
