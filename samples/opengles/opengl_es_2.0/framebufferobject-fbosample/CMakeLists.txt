
# framebufferobject-fbosample - render-to-texture via FBO (OpenGL ES 2.0)
# Ported from ssw-mktg/GPU_SDK_OpenGLES/OpenGL_ES_2.0/FrameBufferObject-FBOSample.

file(GLOB FRAMEBUFFEROBJECT-FBOSAMPLE_SOURCES
	"*.cpp"
)

add_executable(framebufferobject-fbosample ${FRAMEBUFFEROBJECT-FBOSAMPLE_SOURCES})

target_link_libraries(framebufferobject-fbosample PRIVATE
	SdkGlesFramework
)

target_include_directories(framebufferobject-fbosample PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(framebufferobject-fbosample PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(framebufferobject-fbosample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET framebufferobject-fbosample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")
add_custom_command(TARGET framebufferobject-fbosample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${GLES_FONT_SHADERS_DIR}"
		"${_outdir}/FontShaders")

install(TARGETS framebufferobject-fbosample
	RUNTIME DESTINATION bin/framebufferobject-fbosample)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/framebufferobject-fbosample)
install(DIRECTORY "${GLES_FONT_SHADERS_DIR}"
	DESTINATION bin/framebufferobject-fbosample)
