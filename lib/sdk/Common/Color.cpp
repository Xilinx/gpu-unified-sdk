/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "Color.hpp"

#include <fstream>

#include <Common/Log.hpp>




struct bitmapMagic
{
	unsigned char magic[2];
};

struct bitmapHeader
{
	uint32_t fileSize; /**< \brief Total size of the bitmap in bytes. */
	uint16_t creator1; /**< \brief Reserved field which can be application defined. */
	uint16_t creator2; /**< \brief Reserved field which can be application defined. */
	uint32_t offset; /**< \brief Offset in bytes to the beginning of the image data block. */
};

struct bitmapInformationHeader
{
	uint32_t size; /**< Size of the information headers in bytes. */
	int32_t width; /**< Width of the image. */
	int32_t height; /**< Height of the image. */
	uint16_t numberOfColorPlanes; /**< The number of colour planes. The only legal value is 1. */
	uint16_t bitsPerPixel; /**< Number of bits per pixel in the image. */
	uint32_t compressionType; /**< Compression type. Use 0 for uncompressed. */
	uint32_t rawBitmapSize; /**< Size of the image data including padding (does not include the size of the headers). */
	int32_t horizontalResolution; /**< Resolution is in pixels per meter. */
	int32_t verticalResolution; /**< Resolution is in pixels per meter. */
	uint32_t numberOfColors; /**< Number of colours in the image, can be left as 0. */
	uint32_t numberOfImportantColors; /**< Generally ignored by applications. */
};




void saveToBitmap(std::string filename, int width, int height, const unsigned char* imageData)
{
	/* Try and open the file for writing. */
	std::fstream imageFile(filename.c_str(), std::ios::out);
	CHECK(imageFile.is_open(), "Unable to open:", filename)

	/* Magic header bits come from the bitmap specification. */
	const struct bitmapMagic magic = { {0x42, 0x4d} };
	struct bitmapHeader header;
	struct bitmapInformationHeader informationHeader;

	/*
	 * Each row of the data must be padded to a multiple of 4 bytes according to the bitmap specification.
	 * This method uses three bytes per pixel (hence the width * 3).
	 * Then we increase the padding until it is divisible by 4.
	 */
	int paddedWidth = width * 3;
	while((paddedWidth % 4) != 0)
	{
		paddedWidth++;
	}

	/* Setup the bitmap header. */
	header.fileSize = sizeof(magic) + sizeof(header) + sizeof(informationHeader) + paddedWidth * height;
	header.creator1 = 0;
	header.creator2 = 0;
	header.offset = sizeof(magic) + sizeof(header) + sizeof(informationHeader);

	/* Setup the bitmap information header. */
	informationHeader.size = sizeof(informationHeader);
	informationHeader.width = width;
	informationHeader.height = height;
	informationHeader.numberOfColorPlanes = 1;
	informationHeader.bitsPerPixel = 24;
	informationHeader.compressionType = 0;
	informationHeader.rawBitmapSize = paddedWidth * height;
	informationHeader.horizontalResolution = 2835;
	informationHeader.verticalResolution = 2835;
	informationHeader.numberOfColors = 0;
	informationHeader.numberOfImportantColors = 0;

	/* Try to write the header data. */
	CHECK(!imageFile.write((char*)&magic, sizeof(magic)).bad() &&
			!imageFile.write((char*)&header, sizeof(header)).bad() &&
			!imageFile.write((char*)&informationHeader, sizeof(informationHeader)).bad(),
			"Failed to write bitmap header")

	for (int y = height - 1; y >= 0; y--)
	{
		for (int x = 0; x < width; x++)
		{
			/* The pixels lie in RGB order in memory, we need to store them in BGR order. */
			unsigned char rgb[3];
			rgb[2] = imageData[3 * (y * informationHeader.width + x) + 0];
			rgb[1] = imageData[3 * (y * informationHeader.width + x) + 1];
			rgb[0] = imageData[3 * (y * informationHeader.width + x) + 2];

			CHECK(!imageFile.write((char*)&rgb, 3).bad())
		}
		/*
		 * At the end of the row, write out blank bytes to ensure the row length is
		 * a multiple of 4 bytes (part of the bitmap specification).
		 */
		for (int x = width * 3; x < paddedWidth; x++)
		{
			char b = 0;
			CHECK(!imageFile.write(&b, 1).bad())
		}
	}
}


void loadFromBitmap(const std::string filename, int* const width, int* const height, unsigned char **imageData)
{
	/* Try and open the file for reading. */
	std::ifstream imageFile(filename.c_str(), std::ios::in);
	CHECK(imageFile.is_open(), "Unable to open:", filename)

	/*
	 * Read and check the headers to make sure we support the type of bitmap passed in.
	 */
	struct bitmapMagic magic;
	struct bitmapHeader header;
	struct bitmapInformationHeader informationHeader;

	CHECK(!imageFile.read((char*)&magic, sizeof(magic)).bad() &&
			magic.magic[0] == 0x42 && magic.magic[1] == 0x4d,
			"Not a valid BMP file header")

	/* 54 is the standard size of a bitmap header. */
	CHECK(!imageFile.read((char*)&header, sizeof(header)).bad() && header.offset == 54,
			"Not a supported BMP format")

	CHECK(!imageFile.read((char*)&informationHeader, sizeof(informationHeader)).bad() &&
			informationHeader.compressionType == 0 &&
			informationHeader.bitsPerPixel == 24,
			"We only support uncompressed 24-bits per pixel RGB")

	int row_delta;
	int first_row;
	int after_last_row;
	if (informationHeader.height > 0)
	{
		/* The image is stored upside down in memory */
		row_delta = -1;
		first_row = informationHeader.height - 1;
		after_last_row = -1;
	}
	else
	{
		informationHeader.height = -informationHeader.height;
		row_delta = 1;
		first_row = 0;
		after_last_row = informationHeader.height;
	}

	/* Calculate the paddle of the image to skip it when reading the buffer. */
	int paddedWidth = informationHeader.width * 3;
	while((paddedWidth % 4) != 0)
	{
		paddedWidth++;
	}

	/* 24-bits per pixel means 3 bytes of data per pixel. */
	int size = 3 * paddedWidth * informationHeader.height;
	std::unique_ptr<uint8_t[]> readBuffer(new uint8_t[size]);

	/* Try to read in the image data. */
	CHECK(!imageFile.read((char*)readBuffer.get(), size).bad(),
			"Error reading main image data")

	*imageData = new unsigned char[size];

	int readBufferIndex = 0;
	/* Loop throught the image data and store it at the output data location. */
	for (int y = first_row; y != after_last_row; y += row_delta)
	{
		for (int x = 0; x < informationHeader.width; x++)
		{
			/* The pixels lie in BGR order, we need to resort them into RGB */
			(*imageData)[3 * (y * informationHeader.width + x) + 0] = readBuffer[readBufferIndex + 2];
			(*imageData)[3 * (y * informationHeader.width + x) + 1] = readBuffer[readBufferIndex + 1];
			(*imageData)[3 * (y * informationHeader.width + x) + 2] = readBuffer[readBufferIndex + 0];

			readBufferIndex += 3;
		}
		/* Skip padding. */
		readBufferIndex += paddedWidth - (informationHeader.width * 3);
	}

	*width  = informationHeader.width;
	*height = informationHeader.height;
}


void luminanceToRGB(const unsigned char* luminanceData, unsigned char* rgbData, int width, int height)
{
	ASSERT(luminanceData)
	ASSERT(rgbData)

	for (int n = width * height - 1; n >= 0; --n)
	{
		unsigned char d = luminanceData[n];
		rgbData[3 * n + 0] = d;
		rgbData[3 * n + 1] = d;
		rgbData[3 * n + 2] = d;
	}
}


void RGBToLuminance(const unsigned char* const rgbData, unsigned char* const luminanceData, int width, int height)
{
	ASSERT(rgbData)
	ASSERT(luminanceData)

	for (int n = width * height - 1; n >= 0; --n)
	{
		float r = rgbData[3 * n + 0];
		float g = rgbData[3 * n + 1];
		float b = rgbData[3 * n + 2];
		luminanceData[n] = (unsigned char) (0.2126f * r + 0.7152f * g + 0.0722f * b);
	}
}


void RGBToRGBA(const unsigned char* const rgbData, unsigned char* const rgbaData, int width, int height)
{
	ASSERT(rgbData)
	ASSERT(rgbaData)

	for (int n = 0; n < width * height; n++)
	{
		/* Copy the RGB components directly. */
		rgbaData[4 * n + 0] = rgbData[3 * n + 0];
		rgbaData[4 * n + 1] = rgbData[3 * n + 1];
		rgbaData[4 * n + 2] = rgbData[3 * n + 2];

		/* Set the alpha channel to 255 (fully opaque). */
		rgbaData[4 * n + 3] = (unsigned char)255;
	}
}


void RGBAToRGB(const unsigned char* const rgbaData, unsigned char* const rgbData, int width, int height)
{
	ASSERT(rgbaData)
	ASSERT(rgbData)

	for (int n = 0; n < width * height; n++)
	{
		/* Copy the RGB components but throw away the alpha channel. */
		rgbData[3 * n + 0] = rgbaData[4 * n + 0];
		rgbData[3 * n + 1] = rgbaData[4 * n + 1];
		rgbData[3 * n + 2] = rgbaData[4 * n + 2];
	}
}
