/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#include <vulkan-strings/VulkanStrings.hpp>

#include <vulkan/vulkan_core.h>




namespace vk {
namespace detail {


#ifdef VULKAN_STRINGS_GEN
#include <vulkan-strings/gen/strings-implementation.inl>
#endif


std::string_view findValue(const ValueEntry * const entries, const int size, const int32_t value) noexcept
{
	for (int i = 0; i < size; ++i) {
		const ValueEntry & entry = entries[i];
		if (entry.value == value) return entry.name;
	}
	return {};
}


}} // vk::detail
