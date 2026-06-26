/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include "Geometry.hpp"
#include "Log.hpp"




template <>
struct Log::Write<Size> {
	static void write(std::ostream & s, const Size & size) noexcept
	{
		s << Log::format("(%d x %d)", size.width, size.height);
	}
};




template <>
struct Log::Write<Point> {
	static void write(std::ostream & s, const Point & point) noexcept
	{
		s << Log::format("(%d %d)", point.x, point.y);
	}
};




template <>
struct Log::Write<Rect> {
	static void write(std::ostream & s, const Rect & rect) noexcept
	{
		s << Log::format("(%d %d - %d x %d)", rect.pos.x, rect.pos.y, rect.size.width, rect.size.height);
	}
};




template <>
struct Log::Write<RealSize> {
	static void write(std::ostream & s, const RealSize & size) noexcept
	{
		s << Log::format("(%f x %d)", size.width, size.height);
	}
};
