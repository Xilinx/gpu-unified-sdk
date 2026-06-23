
# bbox - bounding-box rendering with on-screen text (OpenGL ES 2.0)
# Ported from ssw-mktg/GPU_SDK_OpenGLES/OpenGL_ES_2.0/BBOX.

file(GLOB BBOX_SOURCES
	"*.cpp"
)

add_executable(bbox ${BBOX_SOURCES})

target_link_libraries(bbox PRIVATE
	SdkGlesFramework
)

target_include_directories(bbox PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(bbox PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(bbox PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET bbox POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")

install(TARGETS bbox
	RUNTIME DESTINATION bin/bbox)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/bbox)
