
cmake_minimum_required(VERSION 3.22)

project(LibSdk)


set(ECM_DIR       "${ModulesDir}/ecm" CACHE STRING "" FORCE)
set(GStreamer_DIR "${ModulesDir}/gstreamer" CACHE STRING "" FORCE)
set(DRM_DIR       "${ModulesDir}/drm" CACHE STRING "" FORCE)


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


if (Sdk_UseDma)
	target_compile_definitions(sdk_compile_flags INTERFACE
		SDK_USE_DMA
	)
endif()


# common interface properties
add_library(SdkIf INTERFACE)

target_include_directories(SdkIf INTERFACE
	"${ThisDir}"
)


add_subdirectory(Common)
add_subdirectory(OpenCL)
add_subdirectory(Vulkan)
