/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#include "Utils.hpp"

#include <algorithm>

#include <Common/Log.hpp>




Size calculateNv12Size(const Size & rgbSize) noexcept
{
	const Size chromaSize = Size {
		.width  = (rgbSize.width  + 1) / 2,
		.height = (rgbSize.height + 1) / 2,
	};

	const int alignedChromaWidth = alignValue(chromaSize.width, kChromaAlignment);

	return Size {
		.width = alignedChromaWidth * 2,
		.height = chromaSize.height * 2,
	};
}


Size calculateNv12SizeAsRgba(const Size & nv12Size) noexcept
{
	return Size{
		.width = nv12Size.width / 4,
		.height = nv12Size.height / 2 * 3,
	};
}


Size calculateYuyvSizeAsRgba(const Size & nv12Size) noexcept
{
	return Size{
		.width = nv12Size.width / 2,
		.height = nv12Size.height,
	};
}


void splitString(const std::string_view & string, const std::string_view & sep,
		const SplitStringCallback & cb) noexcept
{
	ASSERT(!sep.empty())
	for (std::string_view s = string; !s.empty();) {
		const auto pos = s.find(sep);
		const std::string_view name = pos == std::string_view::npos ?
				s : s.substr(0, pos);
		cb(name);
		if (pos == std::string_view::npos) break;
		s = s.substr(pos + sep.size());
	}
}


std::vector<std::string_view> splitSpaceSeparated(const std::string_view & extensions)
{
	std::vector<std::string_view> list;
	for (std::string_view s = extensions; !s.empty();) {
		const auto fpos = s.find_first_not_of(' ');
		if (fpos == std::string_view::npos) break;
		const auto spos = s.find(' ', fpos);
		const std::string_view name = spos == std::string_view::npos ?
				s.substr(fpos) : s.substr(fpos, spos - fpos);
		if (name.empty()) break;
		list.push_back(name);
		if (spos == std::string_view::npos) break;
		s = s.substr(spos + 1);
	}
	std::sort(list.begin(), list.end());
	return list;
}




Extensions Extensions::fromString(const std::string_view & s) noexcept
{
	return Extensions(splitSpaceSeparated(s));
}


Extensions Extensions::fromList(std::vector<std::string_view> && list) noexcept
{
	std::sort(list.begin(), list.end());
	return Extensions(std::move(list));
}

bool Extensions::contains(const std::string_view & name) const noexcept
{
	return std::find(list_.begin(), list_.end(), name) != list_.end();
}
