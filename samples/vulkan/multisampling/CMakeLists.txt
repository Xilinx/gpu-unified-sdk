# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(multisampling
	main.cpp
)

target_link_libraries(multisampling PRIVATE
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
# Stage the binary and its compiled assets in the shared output folder.
get_filename_component(_sample_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(_outdir "${SDK_OUTPUT_DIR}/vulkan/${_sample_name}")
set_target_properties(multisampling PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${_outdir}")

set(SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BIN_DIR "${_outdir}/assets/shaders")
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

add_custom_target(multisampling_shaders DEPENDS ${SHADER_SPV_FILES})
add_dependencies(multisampling multisampling_shaders)

install(TARGETS multisampling
	RUNTIME DESTINATION bin/multisampling
)

install(FILES ${SHADER_SPV_FILES}
	DESTINATION bin/multisampling/assets/shaders
)

# Copy + install texture assets if present.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures")
	add_custom_command(TARGET multisampling POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
			"${_outdir}/assets/textures")
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
		DESTINATION bin/multisampling/assets)
endif ()
