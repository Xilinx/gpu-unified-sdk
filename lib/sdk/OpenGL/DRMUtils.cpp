/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#include "DRMUtils.hpp"

#include <cstring>

#include <fcntl.h>

#include <sys/mman.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include <drm_fourcc.h>

#include "Log.hpp"




namespace Drm {




Fd openDevice(const std::string_view & path)
{
	const std::string_view devicePath = path.data() ? path : "/dev/dri/card0";

	Fd fd = Fd::adapt(::open(devicePath.data(), O_RDWR | __O_CLOEXEC));
	CHECK(!fd.isNull(), "Error opening:", devicePath, Log::err())

	uint64_t supportsDumb;
	const int ret = ::drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &supportsDumb);
	CHECK(ret == 0 && supportsDumb != 0,
			"DRM_CAP_DUMB_BUFFER failed, no dumb buffers support:", devicePath)

	return fd;
}


Buffer::Handle::Handle(const int devFd, const uint32_t handle) noexcept :
	devFd(devFd),
	handle(handle)
{
}


Buffer::Handle::~Handle() noexcept
{
	if (handle != 0) {
		::drm_mode_destroy_dumb dreq;
		std::memset(&dreq, 0, sizeof(dreq));
		dreq.handle = handle;
		::drmIoctl(devFd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	}
}


Buffer::Buffer(const int devFd, Handle && handle, Dma::Buffer && dmaBuffer) noexcept :
	devFd_(devFd),
	handle_(std::move(handle)),
	dmaBuffer_(std::move(dmaBuffer))
{
}


Buffer::~Buffer() noexcept
{
}


Buffer Buffer::null() noexcept
{
	return Buffer(Fd::kNullFd, Handle(Fd::kNullFd, 0), Dma::Buffer::null());
}


Buffer Buffer::create(const int devFd, const Dma::Buffer::Params & params)
{
	int ret;

	::drm_mode_create_dumb creq;
	std::memset(&creq, 0, sizeof(creq));
	creq.width  = params.size.width;
	creq.height = params.size.height;
	creq.bpp    = bitsPerPixelForFormat(params.pixelFormat);
	ret = ::drmIoctl(devFd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	CHECK(ret == 0, "DRM_IOCTL_MODE_CREATE_DUMB failed:", Log::err())

	Handle handle(devFd, creq.handle);

	::drm_prime_handle prime;
	std::memset(&prime, 0, sizeof(prime));
	prime.handle = handle.handle;
	ret = ::drmIoctl(devFd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &prime);
	CHECK(ret == 0, "DRM_IOCTL_PRIME_HANDLE_TO_FD failed:", Log::err())

	Fd dmabufFd = Fd::adapt(prime.fd);

	::drm_mode_map_dumb mreq;
	std::memset(&mreq, 0, sizeof(mreq));
	mreq.handle = handle.handle;
	ret = ::drmIoctl(devFd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	CHECK(ret == 0, "Failed to map DRM:", Log::err())

	void * const data = ::mmap(0, creq.size, PROT_READ | PROT_WRITE, MAP_SHARED,
			devFd, mreq.offset);
	CHECK(data != MAP_FAILED, "Failed to mmap DRM memory:", Log::err())

	auto mapped = Mapped<uint8_t>::adapt(data, creq.size);

	Dma::Buffer dmaBuffer = Dma::Buffer::adapt(params, std::move(dmabufFd), std::move(mapped),
			creq.pitch);

	return Buffer(devFd, std::move(handle), std::move(dmaBuffer));
}


Buffer Buffer::adapt(int devFd, Handle && handle, const Dma::Buffer::Params & params,
		Fd && dmabufFd, const int64_t dataSize, const int pitch)
{
	int ret;

#if 0
	::drm_prime_handle prime;
	std::memset(&prime, 0, sizeof(prime));
	prime.handle = handle.handle;
	ret = ::drmIoctl(devFd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &prime);
	if (ret != 0) [[unlikely]] {
		const int err = errno;
		THROW(Log::attach(err), "Failed to get DRM fd:", err, std::strerror(err))
	}
	dmabufFd = Fd::adapt(prime.fd);
#endif

#if 0
	::drm_prime_handle prime;
	std::memset(&prime, 0, sizeof(prime));
	prime.handle = handle.handle;
	ret = ::drmIoctl(devFd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &prime);
	if (ret != 0) [[unlikely]] {
		const int err = errno;
		THROW(Log::attach(err), "Failed to get DRM fd:", err, std::strerror(err))
	}
	dmabufFd = Fd::adapt(prime.fd);
#endif

#if 0
	::drm_syncobj_handle freq;
	std::memset(&freq, 0, sizeof(freq));
	freq.fd = -1;
	freq.handle = handle.handle;
	ret = ::drmIoctl(devFd, DRM_IOCTL_SYNCOBJ_HANDLE_TO_FD, &freq);
	CHECK(ret == 0, "DRM_IOCTL_SYNCOBJ_HANDLE_TO_FD failed:", Log::err())

	::drm_syncobj_handle hreq;
	std::memset(&hreq, 0, sizeof(hreq));
	hreq.fd = dmabufFd;
	ret = ::drmIoctl(devFd, DRM_IOCTL_SYNCOBJ_FD_TO_HANDLE, &hreq);
	CHECK(ret == 0, "DRM_IOCTL_SYNCOBJ_FD_TO_HANDLE failed:", Log::err())
	DEBUG("handle:", hreq.handle, handle.handle)
#endif

	::drm_mode_map_dumb mreq;
	std::memset(&mreq, 0, sizeof(mreq));
	mreq.handle = handle.handle;
	ret = ::drmIoctl(devFd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	CHECK(ret == 0, "Failed to map DRM:", Log::err())

	void * const data = ::mmap(0, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED,
			devFd, mreq.offset);
	CHECK(data != MAP_FAILED, "Failed to mmap DRM memory:", Log::err())

	auto mapped = Mapped<uint8_t>::adapt(data, dataSize);

	Dma::Buffer dmaBuffer = Dma::Buffer::adapt(params, std::move(dmabufFd), std::move(mapped),
			pitch);

	return Buffer(devFd, std::move(handle), std::move(dmaBuffer));
}


void Buffer::dump(const std::string_view & name) const noexcept
{
	dmaBuffer_.dump(name);
	DEBUG("    drm handle     :", handle())
}




Res Res::get(const int fd)
{
	const ::drmModeResPtr handle = ::drmModeGetResources(fd);
	CHECK(handle, Log::err())
	return Res(handle);
}


Res::~Res() noexcept
{
	if (handle_) {
		::drmModeFreeResources(handle_);
	}
}




Connector Connector::get(const int fd, const Res & res, const int id)
{
	const ::drmModeConnectorPtr handle = ::drmModeGetConnector(fd, res->connectors[id]);
	CHECK(handle, Log::err())
	return Connector(handle);
}


Connector::~Connector() noexcept
{
	if (handle_) {
		::drmModeFreeConnector(handle_);
	}
}




Encoder Encoder::get(const int fd, const uint32_t id)
{
	const ::drmModeEncoderPtr handle = ::drmModeGetEncoder(fd, id);
	CHECK(handle, Log::err())
	return Encoder(handle);
}


Encoder::~Encoder() noexcept
{
	if (handle_) {
		::drmModeFreeEncoder(handle_);
	}
}



Plane Plane::get(const int fd, uint32_t id)
{
	const ::drmModePlanePtr handle = ::drmModeGetPlane(fd, id);
	CHECK(handle, Log::err())
	return Plane(handle);
}


Plane::~Plane() noexcept
{
	if (handle_) {
		::drmModeFreePlane(handle_);
	}
}




PlaneRes PlaneRes::get(const int fd)
{
	const ::drmModePlaneResPtr handle = ::drmModeGetPlaneResources(fd);
	CHECK(handle, Log::err())
	return PlaneRes(handle);
}


PlaneRes::~PlaneRes() noexcept
{
	if (handle_) {
		::drmModeFreePlaneResources(handle_);
	}
}




ObjectProperties ObjectProperties::get(const int fd, const uint32_t id, const uint32_t type)
{
	const ::drmModeObjectPropertiesPtr handle =
			::drmModeObjectGetProperties(fd, id, type);
	CHECK(handle, Log::err())
	return ObjectProperties(handle);
}


ObjectProperties::~ObjectProperties() noexcept
{
	if (handle_) {
		::drmModeFreeObjectProperties(handle_);
	}
}




Property Property::get(const int fd, const uint32_t prop)
{
	const ::drmModePropertyPtr handle = ::drmModeGetProperty(fd, prop);
	CHECK(handle)
	return Property(handle);
}


Property::~Property() noexcept
{
	if (handle_) {
		::drmModeFreeProperty(handle_);
	}
}




Crtc Crtc::get(const int fd, const uint32_t id)
{
	const ::drmModeCrtcPtr handle = ::drmModeGetCrtc(fd, id);
	CHECK(handle)
	return Crtc(handle);
}


Crtc::~Crtc() noexcept
{
	if (handle_) {
		::drmModeFreeCrtc(handle_);
	}
}




} // Drm
