
#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>

#include "Geometry.hpp"




struct PngLoadMemory {
	uint8_t * data;
	int pitch;
};

enum class PngColorType {
	Rgb,
	Rgba,
	Gray,
};

enum class PngFlip {
	No,
	Yes,
};

using LoadPngCallback = std::function<PngLoadMemory(const Size & size)>;

extern void loadPng(const std::filesystem::path & path, const LoadPngCallback & cb);

extern void dumpPng(const std::filesystem::path & path, const Size & size, int pitch,
		const uint8_t * pixels, PngColorType colorType, PngFlip flip);
