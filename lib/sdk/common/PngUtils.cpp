
#include "PngUtils.hpp"

#include <png.h>

#include "FileUtils.hpp"
#include "Log.hpp"




void loadPng(const std::filesystem::path & path, const LoadPngCallback & cb)
{
	const File file = openFile(path, "rb");

	struct PngReadDeleter {
		png_infop info;
		void operator()(png_structp png) noexcept { png_destroy_read_struct(&png, &info, nullptr); }
	};
	using PngRead = std::unique_ptr<png_struct, PngReadDeleter>;

	const PngRead png = [] () -> PngRead {
		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		CHECK(png)

		const png_infop info = png_create_info_struct(png);
		if (!info) [[unlikely]] {
			png_destroy_read_struct(&png, nullptr, nullptr);
			THROW()
		}

		return PngRead(png, PngReadDeleter{ .info = info });
	}();

	const png_infop info = png.get_deleter().info;

	CHECK(setjmp(png_jmpbuf(png.get())) == 0)

	png_init_io(png.get(), file.get());

	png_read_info(png.get(), info);

	const Size size = Size{
		.width  = int(png_get_image_width(png.get(), info)),
		.height = int(png_get_image_height(png.get(), info)),
	};
	const png_byte colorType = png_get_color_type(png.get(), info);
	const png_byte bitDepth  = png_get_bit_depth(png.get(), info);

	// read only into 8 bit depth
	if (bitDepth == 16) {
		png_set_strip_16(png.get());
	}

	if (colorType == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png.get());
	}

	if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8) {
		png_set_expand_gray_1_2_4_to_8(png.get());
	}

	if (png_get_valid(png.get(), info, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png.get());
	}

	// will alpha with 0xff
	if (colorType == PNG_COLOR_TYPE_RGB ||
			colorType == PNG_COLOR_TYPE_GRAY ||
			colorType == PNG_COLOR_TYPE_PALETTE) {
		png_set_filler(png.get(), 0xff, PNG_FILLER_AFTER);
	}

	if (colorType == PNG_COLOR_TYPE_GRAY ||
			colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png.get());
	}

	png_read_update_info(png.get(), info);

	const PngLoadMemory m = cb(size);

	std::vector<png_bytep> rows = [&] () -> std::vector<png_bytep> {
		std::vector<png_bytep> rows(size.height);
		for (int i = 0; i < size.height; ++i) {
			rows[size.height - 1 - i] = m.data + i * m.pitch;
		}
		return rows;
	}();

	png_read_image(png.get(), rows.data());
}


void dumpPng(const std::filesystem::path & path, const Size & size, const int pitch,
		const uint8_t * const pixels, const PngColorType colorType, const PngFlip flip)
{
	const File file = openFile(path, "wb");

	struct PngWriteDeleter {
		png_infop info;
		void operator()(png_structp png) noexcept { png_destroy_write_struct(&png, &info); }
	};
	using PngWrite = std::unique_ptr<png_struct, PngWriteDeleter>;

	const PngWrite png = [] () -> PngWrite {
		png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				nullptr, nullptr, nullptr);
		CHECK(png)

		const png_infop info = png_create_info_struct(png);
		if (!info) [[unlikely]] {
			png_destroy_write_struct(&png, nullptr);
			THROW()
		}

		return PngWrite(png, PngWriteDeleter{ .info = info });
	}();

	const png_infop info = png.get_deleter().info;

	int e = setjmp(png_jmpbuf(png.get()));
	CHECK(e == 0)

	png_init_io(png.get(), file.get());

	e = setjmp(png_jmpbuf(png.get()));
	CHECK(e == 0)

	const int ct = [colorType] () -> int {
		switch (colorType) {
		case PngColorType::Rgb:  return PNG_COLOR_TYPE_RGB;
		case PngColorType::Rgba: return PNG_COLOR_TYPE_RGB_ALPHA;
		case PngColorType::Gray: return PNG_COLOR_TYPE_GRAY;
		}
		FATAL()
	}();

	png_set_IHDR(png.get(), info, size.width, size.height,
			8, ct, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png.get(), info);

	e = setjmp(png_jmpbuf(png.get()));
	CHECK(e == 0)

	std::vector<png_bytep> rows = [&] () -> std::vector<png_bytep> {
		std::vector<png_bytep> rows(size.height);
		for (int i = 0; i < size.height; ++i) {
			const int y = flip == PngFlip::Yes ? size.height - 1 - i : i;
			rows[y] = const_cast<uint8_t*>(pixels) + i * pitch;
		}
		return rows;
	}();
	png_write_image(png.get(), rows.data());

	e = setjmp(png_jmpbuf(png.get()));
	CHECK(e == 0)

	png_write_end(png.get(), nullptr);
}
