
add_executable(triangle
	main.cpp
)

target_link_libraries(triangle PRIVATE
	SdkCommon
	SdkVulkan
)

# --- Build-time GLSL -> SPIR-V compilation -----------------------------------
find_program(GLSLANG_VALIDATOR glslangValidator)
find_program(GLSLC glslc)

if (GLSLANG_VALIDATOR)
	set(GLSL_COMPILE_CMD ${GLSLANG_VALIDATOR} -V)
elseif (GLSLC)
	set(GLSL_COMPILE_CMD ${GLSLC})
else ()
	message(FATAL_ERROR
		"No GLSL->SPIR-V compiler found. Install glslangValidator or glslc.")
endif ()

# Compile every shader under shaders/ into <binary_dir>/assets/shaders so the
# executable can load them in-place (asset loader resolves "<exe_dir>/assets/").
set(SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders")
file(MAKE_DIRECTORY "${SHADER_BIN_DIR}")

file(GLOB SHADER_SOURCES
	"${SHADER_SRC_DIR}/*.vert"
	"${SHADER_SRC_DIR}/*.frag"
	"${SHADER_SRC_DIR}/*.comp")

set(SHADER_SPV_FILES)
foreach (shader ${SHADER_SOURCES})
	get_filename_component(name "${shader}" NAME)
	set(spv "${SHADER_BIN_DIR}/${name}.spv")
	add_custom_command(
		OUTPUT "${spv}"
		COMMAND ${GLSL_COMPILE_CMD} -o "${spv}" "${shader}"
		DEPENDS "${shader}"
		COMMENT "Compiling shader ${name}"
		VERBATIM
	)
	list(APPEND SHADER_SPV_FILES "${spv}")
endforeach ()

add_custom_target(triangle_shaders DEPENDS ${SHADER_SPV_FILES})
add_dependencies(triangle triangle_shaders)

install(TARGETS triangle
	RUNTIME DESTINATION bin/triangle
)

install(FILES ${SHADER_SPV_FILES}
	DESTINATION bin/triangle/assets/shaders
)

# Copy + install texture assets if present.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures")
	add_custom_command(TARGET triangle POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
			"${CMAKE_CURRENT_BINARY_DIR}/assets/textures")
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
		DESTINATION bin/triangle/assets)
endif ()
