/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "Image.hpp"

#include "Log.hpp"




Image Image::create(const PixelFormat format, const Size & size, const int pitch)
{
	Image image {
		.format = format,
		.size = size,
		.pitch = pitch,
	};

	const int bpp = bitsPerPixelForFormat(format);
	if (pitch != -1) {
		CHECK(pitch * 8 >= size.width * bpp, bpp, pitch)
	} else {
		image.pitch = size.width * bpp / 8;
	}

	image.data.reset(new uint8_t[image.pitch * image.size.height]);

	return image;
}
