/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "DMAUtils.hpp"

#include <cstring>

#include <fcntl.h>

#include <sys/mman.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include <drm_fourcc.h>

#include "Log.hpp"




namespace Dma {


inline Buffer::Buffer(const Params & params, Fd && fd,
		Mapped<uint8_t> && mapped, const int pitch) noexcept :
	params_(params),
	fd_(std::move(fd)),
	mapped_(std::move(mapped)),
	pitch_(pitch)
{
}


Buffer::~Buffer() noexcept
{
}


Buffer Buffer::null() noexcept
{
	return Buffer(Params{}, Fd::null(), Mapped<uint8_t>::null(), 0);
}


Buffer Buffer::adapt(const Params & params, Fd && fd,
		Mapped<uint8_t> && mapped, const int pitch) noexcept
{
	return Buffer(params, std::move(fd), std::move(mapped), pitch);
}


void Buffer::dump(const std::string_view & name) const noexcept
{
	DEBUG("DMA buffer:", name)
	DEBUG("    width x height :", size().width, "x", size().height)
	DEBUG("    pixel format   :", Log::fourcc(uint32_t(pixelFormat())))
	DEBUG("    bits per pixel :", bitsPerPixelForFormat(pixelFormat()))
	DEBUG("    data size      :", dataSize())
	DEBUG("    pitch          :", pitch())
	DEBUG("    stride         :", stride())
}


}
