
set(drm_dir "${ThirdPartyDir}/drm")

find_path(DrmLocation drm.h PATH_SUFFIXES libdrm)
if (NOT DrmLocation)
	message(FATAL_ERROR "libdrm headers not found")
endif()

add_library(drm INTERFACE)

target_include_directories(drm INTERFACE "${DrmLocation}")

target_link_libraries(drm INTERFACE drm)
