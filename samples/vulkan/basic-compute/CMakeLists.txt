# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


add_executable(basic_compute
	main.cpp
)

target_link_libraries(basic_compute PRIVATE
	SdkCommon
	SdkVulkan
)

# --- Build-time GLSL -> SPIR-V compilation -----------------------------------
# Prefer glslangValidator, fall back to glslc. Both ship with the Vulkan SDK.
find_program(GLSLANG_VALIDATOR glslangValidator)
find_program(GLSLC glslc)

if (GLSLANG_VALIDATOR)
	set(GLSL_COMPILE_CMD ${GLSLANG_VALIDATOR} -V)
	set(GLSL_OUTPUT_FLAG -o)
elseif (GLSLC)
	set(GLSL_COMPILE_CMD ${GLSLC})
	set(GLSL_OUTPUT_FLAG -o)
else ()
	message(FATAL_ERROR
		"No GLSL->SPIR-V compiler found. Install glslangValidator or glslc "
		"(part of the Vulkan SDK / glslang package).")
endif ()

# Place compiled SPIR-V under <binary_dir>/assets/shaders so the executable can
# load them in-place (the asset loader resolves "<exe_dir>/assets/<path>"), and
# install the same layout next to the installed binary.
set(SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders")
file(MAKE_DIRECTORY "${SHADER_BIN_DIR}")

set(SHADER_SPV_FILES)
foreach (shader particle.comp particle.vert particle.frag)
	set(src "${SHADER_SRC_DIR}/${shader}")
	set(spv "${SHADER_BIN_DIR}/${shader}.spv")
	add_custom_command(
		OUTPUT "${spv}"
		COMMAND ${GLSL_COMPILE_CMD} ${GLSL_OUTPUT_FLAG} "${spv}" "${src}"
		DEPENDS "${src}"
		COMMENT "Compiling shader ${shader}"
		VERBATIM
	)
	list(APPEND SHADER_SPV_FILES "${spv}")
endforeach ()

add_custom_target(basic_compute_shaders DEPENDS ${SHADER_SPV_FILES})
add_dependencies(basic_compute basic_compute_shaders)

install(TARGETS basic_compute
	RUNTIME DESTINATION bin/basic_compute
)

# The framework resolves shader paths as "<exe_dir>/assets/<path>", so install
# the compiled SPIR-V under assets/shaders.
install(FILES ${SHADER_SPV_FILES}
	DESTINATION bin/basic_compute/assets/shaders
)
