/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include "Fd.hpp"
#include "Geometry.hpp"
#include "Mapped.hpp"
#include "PixelFormat.hpp"




namespace Dma {


class Buffer {
public:
	struct Params {
		Size size = Size::zero();
		PixelFormat pixelFormat = PixelFormat::Invalid;
	};

	static int calculateStride(int width, int bitsPerPixel) noexcept
	{ return width * bitsPerPixel / 8; }

	static Buffer null() noexcept;
	static Buffer adapt(const Params & params, Fd && fd,
			Mapped<uint8_t> && mapped, int pitch) noexcept;

	~Buffer() noexcept;

	Buffer(Buffer && other) noexcept;
	Buffer & operator=(Buffer && other) noexcept;

	const Size & size() const noexcept { return params_.size; }
	PixelFormat pixelFormat() const noexcept { return params_.pixelFormat; }

	int stride() const noexcept { return calculateStride(params_.size.width,
			bitsPerPixelForFormat(params_.pixelFormat)); }

	int pitch() const noexcept { return pitch_; }
	const Fd & fd() const noexcept { return fd_; }
	uint8_t * data() const noexcept { return mapped_.data(); }
	int64_t dataSize() const noexcept { return mapped_.size(); }

	bool isNull() const noexcept { return fd_.isNull(); }
	void dump(const std::string_view & name) const noexcept;

	operator bool() const noexcept { return !fd_.isNull(); }

	Buffer(const Buffer &) = delete;
	void operator=(const Buffer &) = delete;

private:
	Buffer(const Params & params, Fd && fd, Mapped<uint8_t> && mapped, int pitch) noexcept;

	Params params_;

	Fd fd_;
	Mapped<uint8_t> mapped_;
	int pitch_;  // line width in bytes
};


inline Buffer::Buffer(Buffer && other) noexcept :
	params_(other.params_),
	fd_(std::move(other.fd_)),
	mapped_(std::move(other.mapped_)),
	pitch_(other.pitch_)
{}


inline Buffer & Buffer::operator=(Buffer && other) noexcept
{
	params_ = other.params_;
	std::swap(fd_, other.fd_);
	std::swap(mapped_, other.mapped_);
	pitch_ = other.pitch_;
	return *this;
}


}
