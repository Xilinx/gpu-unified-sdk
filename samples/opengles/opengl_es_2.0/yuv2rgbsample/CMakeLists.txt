
# yuv2rgbsample - YUV-to-RGB colour conversion on GPU (OpenGL ES 2.0)
# Ported from ssw-mktg/GPU_SDK_OpenGLES/OpenGL_ES_2.0/YUV2RGBSample.

file(GLOB YUV2RGBSAMPLE_SOURCES
	"*.cpp"
	"*.c"
)

add_executable(yuv2rgbsample ${YUV2RGBSAMPLE_SOURCES})

target_link_libraries(yuv2rgbsample PRIVATE
	SdkGlesFramework
)

target_include_directories(yuv2rgbsample PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(yuv2rgbsample PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(yuv2rgbsample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH TRUE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET yuv2rgbsample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")
add_custom_command(TARGET yuv2rgbsample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${GLES_FONT_SHADERS_DIR}"
		"${_outdir}/FontShaders")

install(TARGETS yuv2rgbsample
	RUNTIME DESTINATION bin/yuv2rgbsample)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/yuv2rgbsample)
install(DIRECTORY "${GLES_FONT_SHADERS_DIR}"
	DESTINATION bin/yuv2rgbsample)
