
# rotozoom - rotation/zoom texture effect (OpenGL ES 2.0)
file(GLOB ROTOZOOM_SOURCES
	"*.cpp"
)

add_executable(rotozoom ${ROTOZOOM_SOURCES})

target_link_libraries(rotozoom PRIVATE
	SdkGlesFramework
)

target_include_directories(rotozoom PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(rotozoom PRIVATE
	WINDOW_W=${GLES_WINDOW_W}
	WINDOW_H=${GLES_WINDOW_H}
)

# Stage the binary and its assets in a dedicated output directory so the asset
# copies never target the source tree.
set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/run")
set_target_properties(rotozoom PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}"
	INSTALL_RPATH_USE_LINK_PATH FALSE
	BUILD_WITH_INSTALL_RPATH TRUE
)

add_custom_command(TARGET rotozoom POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
		"${_outdir}/Supportingfiles")
add_custom_command(TARGET rotozoom POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${GLES_FONT_SHADERS_DIR}"
		"${_outdir}/FontShaders")

install(TARGETS rotozoom
	RUNTIME DESTINATION bin/rotozoom)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Supportingfiles"
	DESTINATION bin/rotozoom)
install(DIRECTORY "${GLES_FONT_SHADERS_DIR}"
	DESTINATION bin/rotozoom)
