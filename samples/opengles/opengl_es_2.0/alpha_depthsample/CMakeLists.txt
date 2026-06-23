
# alpha_depthsample - alpha blending and depth testing (OpenGL ES 2.0)
# Ported from ssw-mktg/GPU_SDK_OpenGLES/OpenGL_ES_2.0/Alpha_DepthSample.

file(GLOB ALPHA_DEPTHSAMPLE_SOURCES
	"*.cpp"
)

add_executable(alpha_depthsample ${ALPHA_DEPTHSAMPLE_SOURCES})

target_link_libraries(alpha_depthsample PRIVATE
	SdkGlesFramework
)

target_include_directories(alpha_depthsample PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(alpha_depthsample PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(alpha_depthsample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET alpha_depthsample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")
add_custom_command(TARGET alpha_depthsample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${GLES_FONT_SHADERS_DIR}"
		"${_outdir}/FontShaders")

install(TARGETS alpha_depthsample
	RUNTIME DESTINATION bin/alpha_depthsample)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/alpha_depthsample)
install(DIRECTORY "${GLES_FONT_SHADERS_DIR}"
	DESTINATION bin/alpha_depthsample)
