
cmake_minimum_required(VERSION 3.22)

project(VulkanVendorIds)


# dependencies
get_filename_component(code_dir "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(pci_ids_file "${code_dir}/3rdparty/pciids/pci.ids")
set(vk_xml_file "${code_dir}/3rdparty/vulkan-headers/registry/vk.xml")

set(script "${CMAKE_CURRENT_SOURCE_DIR}/gen-vulkan-vendor-ids.py")
set(gen_include_dir "${CMAKE_CURRENT_BINARY_DIR}/gen/include")
set(gen_inl_file "${gen_include_dir}/vulkan-vendor-ids/gen/vendorids.inl")

add_custom_command(
	OUTPUT
		"${gen_inl_file}"
	COMMAND
		"${script}"
			--pciids "${pci_ids_file}"
			--vkxml "${vk_xml_file}"
			--output "${gen_inl_file}"
	DEPENDS
		"${script}"
		"${pci_ids_file}"
		"${vk_xml_file}"
)

add_library(VulkanVendorIds STATIC)

target_sources(VulkanVendorIds PRIVATE
	src/VulkanVendorIds.cpp
	"${gen_inl_file}"
)

set_source_files_properties(src/VulkanVendorIds.cpp PROPERTIES
	COMPILE_OPTIONS -Wno-trigraphs
)

target_include_directories(VulkanVendorIds PRIVATE
	"${gen_include_dir}"
)

target_include_directories(VulkanVendorIds PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(VulkanVendorIds PRIVATE
	VULKAN_VENDOR_IDS_GEN
)
