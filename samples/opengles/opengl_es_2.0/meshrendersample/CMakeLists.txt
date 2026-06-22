
# meshrendersample - OBJ/3DS mesh loading and lighting (OpenGL ES 2.0)
# Ported from ssw-mktg/GPU_SDK_OpenGLES/OpenGL_ES_2.0/MeshRenderSample.

file(GLOB MESHRENDERSAMPLE_SOURCES
	"*.cpp"
)

add_executable(meshrendersample ${MESHRENDERSAMPLE_SOURCES})

target_link_libraries(meshrendersample PRIVATE
	SdkGlesFramework
)

target_include_directories(meshrendersample PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(meshrendersample PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(meshrendersample PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH TRUE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET meshrendersample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")
add_custom_command(TARGET meshrendersample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/models"
		"${_outdir}/models")
add_custom_command(TARGET meshrendersample POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${GLES_FONT_SHADERS_DIR}"
		"${_outdir}/FontShaders")

install(TARGETS meshrendersample
	RUNTIME DESTINATION bin/meshrendersample)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/meshrendersample)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/models"
	DESTINATION bin/meshrendersample)
install(DIRECTORY "${GLES_FONT_SHADERS_DIR}"
	DESTINATION bin/meshrendersample)
