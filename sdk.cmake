# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.


set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(AMD_SDK "" CACHE PATH "")
set(AMD_RECIPE "" CACHE PATH "")

function(__amd_setup_toolchain)
	set(sdk_dir    "$ENV{AMD_SDK}")
	set(recipe_dir "$ENV{AMD_RECIPE}")
	if (sdk_dir)
		set(AMD_SDK "${sdk_dir}" CACHE PATH "" FORCE)
	endif()
	if (recipe_dir)
		set(AMD_RECIPE "${recipe_dir}" CACHE PATH "" FORCE)
	endif()
	if (AMD_SDK AND AMD_RECIPE)
		message(FATAL_ERROR "AMD_SDK and AMD_RECIPE are mutually exclusive")
	endif()
	if (NOT AMD_SDK AND NOT AMD_RECIPE)
		message(FATAL_ERROR
			"Specify path to the PetaLinux BSP SDK or recipe work dir in environment variable:\n"
			"    AMD_SDK=/path/to/petalinux/bsp/images/linux/sdk\n"
			"    or\n"
			"    AMD_RECIPE=/path/to/petalinux/bsp/build/tmp/work/cortexa72-cortexa53-xilinx-linux/myproject/1.0-r0\n"
			"NOTE: To build PetaLinux SDK:\n"
			"    petalinux-build --sdk\n"
			"    petalinux-package --sysroot\n"
		)
	endif()

	# re export env variables for the CMake test subproject, which are configured at build time,
	# when no cache variables present
	if (AMD_SDK)
		set(ENV{AMD_SDK} "${AMD_SDK}")
	endif()
	if (AMD_RECIPE)
		set(ENV{AMD_RECIPE} "${AMD_RECIPE}")
	endif()

	if (AMD_SDK)
		function(get_sysroot_name PATH PREFIX NAME)
			set(suffix "linux")
			file(GLOB sysroots RELATIVE "${PATH}" "${PATH}/${PREFIX}-*-${suffix}")
			set(names)
			foreach (sysroot ${sysroots})
				if (IS_DIRECTORY "${PATH}/${sysroot}")
					string(LENGTH "${sysroot}" sysroot_length)
					string(LENGTH "${PREFIX}" prefix_length)
					string(LENGTH "${suffix}" suffix_length)
					math(EXPR name_begin "${prefix_length} + 1")
					math(EXPR name_length "${sysroot_length} - ${prefix_length} - ${suffix_length} - 2")
					string(SUBSTRING "${sysroot}" ${name_begin} ${name_length} name)
					list(APPEND names "${name}")
				endif()
			endforeach()
			list(LENGTH names name_count)
			if (name_count EQUAL 0)
				message(FATAL_ERROR "No sysroot found at: ${PATH}/${PREFIX}-*")
			endif()
			if (name_count GREATER 1)
				message(FATAL_ERROR "More than 1 sysroot found at: ${PATH}/${PREFIX}-*")
			endif()
			set(${NAME} "${names}" PARENT_SCOPE)
		endfunction()

		get_sysroot_name("${AMD_SDK}/sysroots" "x86_64" host_sysroot_name)
		get_sysroot_name("${AMD_SDK}/sysroots" "cortexa72-cortexa53" target_sysroot_name)

		set(host_sysroot   "${AMD_SDK}/sysroots/x86_64-${host_sysroot_name}-linux")
		set(target_sysroot "${AMD_SDK}/sysroots/cortexa72-cortexa53-${target_sysroot_name}-linux")
		set(toolchain_prefix "${host_sysroot}/usr/bin/aarch64-${target_sysroot_name}-linux/aarch64-${target_sysroot_name}-linux-")
	endif()
	if (AMD_RECIPE)
		set(host_sysroot   "${AMD_RECIPE}/recipe-sysroot-native")
		set(target_sysroot "${AMD_RECIPE}/recipe-sysroot")
		message(FATAL_ERROR "Resolve toolchain_prefix")
	endif()

	set(CMAKE_C_COMPILER   "${toolchain_prefix}gcc" PARENT_SCOPE)
	set(CMAKE_CXX_COMPILER "${toolchain_prefix}g++" PARENT_SCOPE)
	set(CMAKE_ASM_COMPILER "${toolchain_prefix}gcc" PARENT_SCOPE)

	set(CMAKE_ADDR2LINE "${toolchain_prefix}addr2line" CACHE FILEPATH "")
	set(CMAKE_AR        "${toolchain_prefix}gcc-ar"    CACHE FILEPATH "")
	set(CMAKE_LINKER    "${toolchain_prefix}ld"        CACHE FILEPATH "")
	set(CMAKE_NM        "${toolchain_prefix}nm"        CACHE FILEPATH "")
	set(CMAKE_OBJCOPY   "${toolchain_prefix}objcopy"   CACHE FILEPATH "")
	set(CMAKE_OBJDUMP   "${toolchain_prefix}objdump"   CACHE FILEPATH "")
	set(CMAKE_RANLIB    "${toolchain_prefix}ranlib"    CACHE FILEPATH "")
	set(CMAKE_READELF   "${toolchain_prefix}readelf"   CACHE FILEPATH "")
	set(CMAKE_STRIP     "${toolchain_prefix}strip"     CACHE FILEPATH "")

	# only search in the paths provided so cmake doesnt pick
	# up libraries and tools from the native build machine
	set(CMAKE_FIND_ROOT_PATH "${target_sysroot}" PARENT_SCOPE)
	set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY PARENT_SCOPE)
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER PARENT_SCOPE)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY PARENT_SCOPE)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY PARENT_SCOPE)
	set(CMAKE_FIND_USE_CMAKE_PATH NO PARENT_SCOPE)

	# required for pkg-config to properly locate .pc files and return variables relative to the target sysroot
	set(CMAKE_PREFIX_PATH "${target_sysroot}/usr" PARENT_SCOPE)
	set(PKG_CONFIG_ARGN "--define-variable=prefix=${target_sysroot}/usr" CACHE STRING "Arguments to supply to pkg-config" FORCE)

	# extend list of cmake modules
	set(module_path CMAKE_MODULE_PATH "${target_sysroot}/usr/share/cmake/Modules/")
	set(CMAKE_MODULE_PATH "${module_path}" PARENT_SCOPE)

	# populate compiler and linker flags
	set(common_flags
		-mcpu=cortex-a72.cortex-a53
		-march=armv8-a+crc
		-fstack-protector-strong
		--sysroot=${target_sysroot}
		-pipe
		-feliminate-unused-debug-types
		-fdebug-prefix-map=${target_sysroot}=
		-fmacro-prefix-map=${target_sysroot}=
		-fdebug-prefix-map=${host_sysroot}=
	)

	function(export_flags var)
		set(all_flags ${common_flags} ${ARGN})
		list(JOIN all_flags " " flags_str)
		set(${var} "${flags_str}" CACHE STRING "")
	endfunction()

	export_flags(CMAKE_C_FLAGS
	)

	export_flags(CMAKE_CXX_FLAGS
		-fvisibility-inlines-hidden
	)

	export_flags(CMAKE_ASM_FLAGS
	)

	export_flags(CMAKE_C_LINK_FLAGS
		-Wl,-O1
		-Wl,--hash-style=gnu
		-Wl,--as-needed
		-Wl,-z,relro,-z,now
	)

	export_flags(CMAKE_CXX_LINK_FLAGS
		-fvisibility-inlines-hidden
		-Wl,-O1
		-Wl,--hash-style=gnu
		-Wl,--as-needed
		-Wl,-z,relro,-z,now
	)

	set(release_flags
		-O2
		-D_FORTIFY_SOURCE=2
		-DNDEBUG
	)
	list(JOIN release_flags " " release_flags_str)
	set(CMAKE_C_FLAGS_RELEASE   "${release_flags_str}" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_RELEASE "${release_flags_str}" CACHE STRING "")
	set(CMAKE_ASM_FLAGS_RELEASE "${release_flags_str}" CACHE STRING "")
endfunction()

__amd_setup_toolchain()

set(CMAKE_C_COMPILER_LAUNCHER)
set(CMAKE_CXX_COMPILER_LAUNCHER)

set(CMAKE_INSTALL_RPATH)

set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

set(CMAKE_LIBRARY_PATH /usr/lib /lib)

list(APPEND CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES /usr/include)
list(APPEND CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES /usr/include)
