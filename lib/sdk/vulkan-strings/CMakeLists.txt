# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.


cmake_minimum_required(VERSION 3.22)

project(VulkanStrings)

set(CMAKE_CXX_STANDARD 20)


# dependencies
get_filename_component(code_dir "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(vk_xml_file "${code_dir}/3rdparty/vulkan-headers/registry/vk.xml")

set(script "${CMAKE_CURRENT_SOURCE_DIR}/gen-vulkan-strings.py")
set(gen_include_dir "${CMAKE_CURRENT_BINARY_DIR}/gen/include")
set(gen_interface_inl_file "${gen_include_dir}/vulkan-strings/gen/strings-interface.inl")
set(gen_implementation_inl_file "${gen_include_dir}/vulkan-strings/gen/strings-implementation.inl")

add_custom_command(
	OUTPUT
		"${gen_interface_inl_file}"
		"${gen_implementation_inl_file}"
	COMMAND
		"${script}"
			--vkxml "${vk_xml_file}"
			--output-interface "${gen_interface_inl_file}"
			--output-implementation "${gen_implementation_inl_file}"
	DEPENDS
		"${script}"
		"${vk_xml_file}"
)

add_library(VulkanStrings STATIC)

target_sources(VulkanStrings PRIVATE
	src/VulkanStrings.cpp
	"${gen_interface_inl_file}"
	"${gen_implementation_inl_file}"
)

target_include_directories(VulkanStrings PUBLIC
	"${gen_include_dir}"
)

target_include_directories(VulkanStrings PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(VulkanStrings PUBLIC
	VULKAN_STRINGS_GEN
)
