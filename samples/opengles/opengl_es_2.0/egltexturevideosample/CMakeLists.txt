
# egltexturevideosample - EGLImage external texture / video (OpenGL ES 2.0).
# Ported from ssw-mktg/GPU_SDK_OpenGLES/OpenGL_ES_2.0/EGLTextureVideoSample.
#
# Like eglrenderingsample, this legacy sample is an X11/fbdev EGL demo that
# manages its own EGL surface and declares its own local WINDOW_W/WINDOW_H, so
# it is built standalone against X11 + EGL + GLESv2 (matching the legacy
# makefile) rather than linking the Wayland SdkGlesFramework.

find_package(X11 QUIET)
if(NOT X11_FOUND)
	message(STATUS "egltexturevideosample: X11 not found, skipping target")
	return()
endif()

file(GLOB EGLTEXTUREVIDEOSAMPLE_SOURCES
	"*.c"
)

add_executable(egltexturevideosample ${EGLTEXTUREVIDEOSAMPLE_SOURCES})

target_link_libraries(egltexturevideosample PRIVATE
	EGL
	GLESv2
	${X11_LIBRARIES}
	Xext
	pthread
	dl
	m
)

target_include_directories(egltexturevideosample PRIVATE
	${X11_INCLUDE_DIR}
)

set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(egltexturevideosample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

install(TARGETS egltexturevideosample
	RUNTIME DESTINATION bin/egltexturevideosample)
