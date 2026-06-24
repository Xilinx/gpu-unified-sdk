
# eglrenderingsample - EGLImage rendering (OpenGL ES 2.0).#
# This sample is an X11/fbdev EGL demo: it creates its own EGL surface directly
# (X11 Display/Window, fbdev mmap) instead of using the SdkGlesFramework Wayland
# podium. It is therefore built standalone against X11 + EGL + GLESv2 and does
# NOT link SdkGlesFramework. It also declares its own local WINDOW_W/WINDOW_H, so
# the shared window-size defines are intentionally not passed here.

find_package(X11 QUIET)
if(NOT X11_FOUND)
	message(STATUS "eglrenderingsample: X11 not found, skipping target")
	return()
endif()

file(GLOB EGLRENDERINGSAMPLE_SOURCES
	"*.c"
)

add_executable(eglrenderingsample ${EGLRENDERINGSAMPLE_SOURCES})

target_link_libraries(eglrenderingsample PRIVATE
	EGL
	GLESv2
	${X11_LIBRARIES}
	Xext
	pthread
	dl
	m
)

target_include_directories(eglrenderingsample PRIVATE
	${X11_INCLUDE_DIR}
)

set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(eglrenderingsample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

install(TARGETS eglrenderingsample
	RUNTIME DESTINATION bin/eglrenderingsample)
