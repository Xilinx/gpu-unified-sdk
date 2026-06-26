/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once




struct Point {
	int x;
	int y;

	static Point zero() noexcept
	{
		return Point{ .x = 0, .y = 0 };
	}

	bool operator==(const Point & point) const noexcept { return x == point.x && y == point.y; }
};




struct Size {
	int width;
	int height;

	static Size zero() noexcept
	{
		return Size { .width = 0, .height = 0 };
	}

	static Size create(int width, int height) noexcept
	{
		return Size{
			.width  = width,
			.height = height,
		};
	}

	Size operator*(int scale) const noexcept {
		return Size{
			.width  = width  * scale,
			.height = height * scale,
		};
	}

	bool operator==(const Size & size) const noexcept { return width == size.width && height == size.height; }
};




struct Rect {
	Point pos;
	Size size;
};




struct RealPoint {
	float x;
	float y;

	static RealPoint zero() noexcept
	{
		return RealPoint { .x = 0.0f, .y = 0.0f };
	}

	RealPoint operator-(const RealPoint & p) const noexcept {
		return RealPoint{ .x = x - p.x, .y = y - p.y };
	}

	RealPoint operator*(const float s) const noexcept {
		return RealPoint{ .x = x * s, .y = y * s };
	}
};




struct RealSize {
	float width;
	float height;
};
