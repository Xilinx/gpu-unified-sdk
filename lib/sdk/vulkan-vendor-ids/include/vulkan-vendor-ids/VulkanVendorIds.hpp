/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <cstdint>
#include <string_view>




namespace vk {

extern std::string_view findVendorNameForId(uint32_t id) noexcept;

}
