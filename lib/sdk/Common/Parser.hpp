/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <string_view>
#include <system_error>




struct Parser {
	static int getInt(const std::string_view & s);
	static int getInt(const std::string_view & s, std::error_code & err) noexcept;

	static float getFloat(const std::string_view & s);
	static float getFloat(const std::string_view & s, std::error_code & err) noexcept;

	static bool getBool(const std::string_view & s);
	static bool getBool(const std::string_view & s, std::error_code & err) noexcept;
};
