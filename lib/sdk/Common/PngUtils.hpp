/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>

#include "Geometry.hpp"
#include "Image.hpp"




struct PngLoadMemory {
	uint8_t * data;
	int pitch;
};

enum class PngColorType {
	Rgb,
	Rgba,
	Gray,
};

inline static constexpr PngColorType kPngColorTypeNull = PngColorType(-1);

enum class PngFlip {
	No,
	Yes,
};

struct PngLoadParams {
	PngColorType colorType = kPngColorTypeNull;
	PngFlip flip = PngFlip::No;
};

struct PngDumpParams {
	PngColorType colorType = kPngColorTypeNull;
	PngFlip flip = PngFlip::No;
};

using LoadPngCallback = std::function<PngLoadMemory(PixelFormat format, const Size & size)>;

extern void loadPng(const std::filesystem::path & path, PngLoadParams params,
		const LoadPngCallback & cb);
extern Image loadPng(const std::filesystem::path & path, PngLoadParams params);

extern void dumpPng(const std::filesystem::path & path, const Size & size, int pitch,
		const uint8_t * pixels, PngDumpParams params);
extern void dumpPng(const std::filesystem::path & path, const Image & image,
		PngDumpParams params);
