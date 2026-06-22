
# eglpreserve - EGL_BUFFER_PRESERVED swap behaviour (OpenGL ES 2.0)
# Ported from ssw-mktg/GPU_SDK_OpenGLES/OpenGL_ES_2.0/EGLpreserve.

file(GLOB EGLPRESERVE_SOURCES
	"*.cpp"
)

add_executable(eglpreserve ${EGLPRESERVE_SOURCES})

target_link_libraries(eglpreserve PRIVATE
	SdkGlesFramework
)

target_include_directories(eglpreserve PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(eglpreserve PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(eglpreserve PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH TRUE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET eglpreserve POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")

install(TARGETS eglpreserve
	RUNTIME DESTINATION bin/eglpreserve)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/eglpreserve)
