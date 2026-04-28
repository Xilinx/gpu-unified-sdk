
cmake_minimum_required(VERSION 3.22)

project(sdk)

set(CMAKE_CXX_STANDARD 20)


get_filename_component(SdkDir "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(RootDir "${SdkDir}/.." ABSOLUTE)


# options
set(SDK_HOST NO CACHE BOOL "")
set(SDK_DEV NO CACHE BOOL "")
set(SDK_WSI "" CACHE STRING "")
set(SDK_WSI_GLFW_NO_API YES CACHE BOOL "")


# vars
if (SDK_HOST)
	set(Sdk_UseDma NO)
else()
	set(Sdk_UseDma YES)
endif()


find_package(PNG REQUIRED)

# resolve libdrm library
find_path(DrmLocation drm.h PATH_SUFFIXES libdrm)
if (NOT DrmLocation)
	message(FATAL_ERROR "libdrm headers not found")
endif()
add_library(libdrm INTERFACE)
target_include_directories(libdrm INTERFACE "${DrmLocation}")
target_link_libraries(libdrm INTERFACE drm)

# resolve libyuv library

# resolve gstreamer-1.0
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


set(LogRootDir "${CMAKE_CURRENT_LIST_DIR}")


# common interface library
add_library(sdk_compile_flags INTERFACE)

target_compile_options(sdk_compile_flags INTERFACE
	-Werror
	-Wall
)

target_include_directories(sdk_compile_flags INTERFACE
	"${CMAKE_CURRENT_LIST_DIR}"
)

if (SDK_DEV)
	# skip warnings about unused vars in dev mode
	target_compile_options(sdk_compile_flags INTERFACE
		-Wno-error=unused-variable
		-Wno-error=unused-but-set-variable
	)
endif()

if (SDK_HOST)
	target_compile_definitions(sdk_compile_flags INTERFACE
		SDK_SHARE_DIR=\"${DemoDir}\"
		SDK_MODELS_DIR=\"${RootDir}/3rdparty/glmark2/data/models\"
	)
endif()


if (Demo_UseDma)
	target_compile_definitions(sdk_compile_flags INTERFACE
		SDK_USE_DMA
	)
endif()


add_subdirectory("Launcher")
add_subdirectory("Server")
add_subdirectory("Client")
