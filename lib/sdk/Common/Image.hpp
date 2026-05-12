
#pragma once

#include <memory>

#include "Geometry.hpp"
#include "PixelFormat.hpp"




struct Image {
	PixelFormat format;
	Size size;
	int pitch;
	std::unique_ptr<uint8_t[]> data;

	static Image create(PixelFormat format, const Size & size, int pitch = -1);
};
