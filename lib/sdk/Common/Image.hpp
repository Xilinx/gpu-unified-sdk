/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


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
