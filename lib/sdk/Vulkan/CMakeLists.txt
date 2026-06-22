
find_package(Vulkan REQUIRED)

# glm is header-only and provided by the Yocto SDK sysroot (it ships
# usr/lib/cmake/glm/glmConfig.cmake), so resolve it via find_package.
#
# Different glm releases export different things: newer ones provide the
# namespaced imported target glm::glm, older ones a plain "glm" target, and some
# only set GLM_INCLUDE_DIRS. Normalise all of these onto glm::glm so the rest of
# this file (and consumers) can link a single, stable target name.
find_package(glm REQUIRED)
if (NOT TARGET glm::glm)
	if (TARGET glm)
		add_library(glm::glm ALIAS glm)
	else()
		add_library(glm::glm INTERFACE IMPORTED)
		if (GLM_INCLUDE_DIRS)
			target_include_directories(glm::glm INTERFACE ${GLM_INCLUDE_DIRS})
		elseif (DEFINED glm_INCLUDE_DIR)
			target_include_directories(glm::glm INTERFACE ${glm_INCLUDE_DIR})
		endif()
	endif()
endif()

# Wayland client + protocols (for the VK_KHR_wayland_surface backend).
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

# Generate the xdg-shell client protocol (header + private code) with
# wayland-scanner, exactly like the original GPU_SDK_Vulkan wayland backend.
pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
find_program(WAYLAND_SCANNER wayland-scanner)
if (NOT WAYLAND_SCANNER)
	message(FATAL_ERROR "wayland-scanner not found (needed to generate xdg-shell protocol)")
endif()

# The Yocto pkg-config wrapper combines --define-variable=prefix=<sysroot>/usr
# (set by the toolchain file) with PKG_CONFIG_SYSROOT_DIR, which prepends the
# sysroot a second time. Collapse a doubled sysroot prefix so the resulting path
# points at a file that actually exists.
if (DEFINED CMAKE_SYSROOT AND CMAKE_SYSROOT)
	set(_sysroot "${CMAKE_SYSROOT}")
elseif (CMAKE_FIND_ROOT_PATH)
	list(GET CMAKE_FIND_ROOT_PATH 0 _sysroot)
endif()
if (_sysroot)
	string(REPLACE "${_sysroot}${_sysroot}" "${_sysroot}"
		WAYLAND_PROTOCOLS_DIR "${WAYLAND_PROTOCOLS_DIR}")
endif()

set(XDG_SHELL_PROTOCOL "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml")
if (NOT EXISTS "${XDG_SHELL_PROTOCOL}")
	message(FATAL_ERROR
		"xdg-shell.xml not found at:\n  ${XDG_SHELL_PROTOCOL}\n"
		"(resolved wayland-protocols pkgdatadir = ${WAYLAND_PROTOCOLS_DIR})")
endif()
set(XDG_SHELL_HEADER   "${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.h")
set(XDG_SHELL_SOURCE   "${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c")

add_custom_command(
	OUTPUT "${XDG_SHELL_HEADER}"
	COMMAND ${WAYLAND_SCANNER} client-header "${XDG_SHELL_PROTOCOL}" "${XDG_SHELL_HEADER}"
	DEPENDS "${XDG_SHELL_PROTOCOL}"
	COMMENT "Generating XDG shell client protocol header"
	VERBATIM
)
add_custom_command(
	OUTPUT "${XDG_SHELL_SOURCE}"
	COMMAND ${WAYLAND_SCANNER} private-code "${XDG_SHELL_PROTOCOL}" "${XDG_SHELL_SOURCE}"
	DEPENDS "${XDG_SHELL_PROTOCOL}"
	COMMENT "Generating XDG shell protocol source"
	VERBATIM
)

add_library(SdkVulkan SHARED
	VulkanUtils.cpp

	framework/semaphore_manager.cpp
	framework/fence_manager.cpp
	framework/command_buffer_manager.cpp
	framework/context.cpp
	framework/assets.cpp
	framework/thread_pool.cpp
	framework/main_loop.cpp
	framework/platform/wsi.cpp
	framework/platform/wayland.cpp
	framework/platform/display.cpp
	framework/platform/platform.cpp

	"${XDG_SHELL_SOURCE}"
	"${XDG_SHELL_HEADER}"
)

# The framework headers include each other as "framework/..." and
# "platform/...", relative to this directory, so expose both this dir and the
# framework dir on the include path. The generated xdg-shell header lives in the
# build dir.
target_include_directories(SdkVulkan
	PUBLIC
		# Vulkan/ resolves "framework/..." includes; Vulkan/framework resolves
		# "platform/..." includes used inside the framework headers.
		"${CMAKE_CURRENT_LIST_DIR}"
		"${CMAKE_CURRENT_LIST_DIR}/framework"
	PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}"
		# stb_image.h is included as <stb/stb_image.h> by assets.cpp.
		"${CMAKE_CURRENT_LIST_DIR}/../../../3rdparty"
)

# Expose glm to this library and to consumers (the sample includes glm too).
target_link_libraries(SdkVulkan PUBLIC glm::glm)

# wayland-client include dirs from the Yocto pkg-config wrapper can come back
# with the sysroot prefixed twice (see xdg-shell handling above). Add them only
# when they actually exist so a bogus doubled path is never put on the cmd line.
foreach (_wl_inc ${WAYLAND_CLIENT_INCLUDE_DIRS})
	if (_sysroot)
		string(REPLACE "${_sysroot}${_sysroot}" "${_sysroot}" _wl_inc "${_wl_inc}")
	endif()
	if (EXISTS "${_wl_inc}")
		target_include_directories(SdkVulkan PRIVATE "${_wl_inc}")
	endif()
endforeach()

target_link_libraries(SdkVulkan
	PRIVATE
		SdkCommon
		${WAYLAND_CLIENT_LIBRARIES}

	PUBLIC
		Vulkan::Vulkan
)

target_link_directories(SdkVulkan PRIVATE ${WAYLAND_CLIENT_LIBRARY_DIRS})
