/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <algorithm>
#include <functional>
#include <string_view>
#include <vector>

#include "Geometry.hpp"




inline static constexpr int kChromaAlignment = 16;

inline int alignValue(int value, int alignment) noexcept
{
	return (value + alignment - 1) & ~(alignment - 1);
}

extern Size calculateNv12Size(const Size & rgbSize) noexcept;
extern Size calculateNv12SizeAsRgba(const Size & nv12Size) noexcept;
extern Size calculateYuyvSizeAsRgba(const Size & nv12Size) noexcept;

using SplitStringCallback = std::function<void(const std::string_view & s)>;
extern void splitString(const std::string_view & string, const std::string_view & sep,
		const SplitStringCallback & cb) noexcept;

extern std::vector<std::string_view> splitSpaceSeparated(const std::string_view & extensions);


class Extensions {
public:
	static Extensions fromString(const std::string_view & s) noexcept;
	static Extensions fromList(std::vector<std::string_view> && list) noexcept;

	Extensions(Extensions && other) noexcept :
		list_(std::move(other.list_))
	{}

	const std::vector<std::string_view> & list() const noexcept { return list_; }

	bool contains(const std::string_view & name) const noexcept;

private:
	Extensions(std::vector<std::string_view> && list) noexcept :
		list_(std::move(list))
	{}

	std::vector<std::string_view> list_;
};




#define UNUSED(x) (void)x;




[[nodiscard]] constexpr bool fuzzySame(const double p1, const double p2) noexcept
{
	return (std::abs(p1 - p2) * 1000000000000.0 <= std::min(std::abs(p1), std::abs(p2)));
}

[[nodiscard]] constexpr bool fuzzySame(const float p1, const float p2) noexcept
{
	return (std::abs(p1 - p2) * 100000.0f <= std::min(std::abs(p1), std::abs(p2)));
}
