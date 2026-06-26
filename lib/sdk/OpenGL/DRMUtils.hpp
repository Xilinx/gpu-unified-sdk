/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "DMAUtils.hpp"




namespace Drm {




class Buffer {
public:
	struct Handle {
		int devFd;
		uint32_t handle;

		Handle(int devFd, uint32_t handle) noexcept;
		~Handle() noexcept;

		Handle(Handle && other) noexcept :
			devFd(other.devFd),
			handle(std::exchange(other.handle, 0))
		{}

		Handle & operator=(Handle && other) noexcept
		{
			devFd = other.devFd;
			std::swap(handle, other.handle);
			return *this;
		}

		Handle(const Handle &) = delete;
		void operator=(const Handle &) = delete;
	};

	static Buffer null() noexcept;
	static Buffer create(int devFd, const Dma::Buffer::Params & params);
	static Buffer adapt(int devFd, Handle && handle, const Dma::Buffer::Params & params,
			Fd && dmabufFd, int64_t dataSize, int pitch);

	~Buffer() noexcept;

	Buffer(Buffer && other) noexcept;
	Buffer & operator=(Buffer && other) noexcept;

	uint32_t handle() const noexcept { return handle_.handle; }
	const Dma::Buffer & dmaBuffer() const noexcept { return dmaBuffer_; }

	bool isNull() const noexcept { return dmaBuffer_.isNull(); }
	void dump(const std::string_view & name) const noexcept;

	operator bool() const noexcept { return dmaBuffer_; }

	Buffer(const Buffer &) = delete;
	void operator=(const Buffer &) = delete;

private:
	Buffer(int devFd, Handle && handle, Dma::Buffer && dmaBuffer) noexcept;

	int devFd_;
	Handle handle_;
	Dma::Buffer dmaBuffer_;
};




class Res {
public:
	static Res null() noexcept { return Res(nullptr); }

	static Res get(int fd);

	~Res() noexcept;

	Res(Res && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr)) {
	}

	Res & operator=(Res && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::drmModeResPtr() const noexcept { return handle_; }
	::drmModeResPtr operator->() const noexcept { return handle_; }

private:
	Res(::drmModeResPtr handle) noexcept :
		handle_(handle)
	{}

	::drmModeResPtr handle_;
};




class Connector {
public:
	static Connector null() noexcept { return Connector(nullptr); }

	static Connector get(int fd, const Res & res, int id);

	~Connector() noexcept;

	Connector(Connector && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	Connector & operator=(Connector && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::drmModeConnectorPtr() const noexcept { return handle_; }
	::drmModeConnectorPtr operator->() const noexcept { return handle_; }

private:
	Connector(::drmModeConnectorPtr handle) noexcept :
		handle_(handle)
	{}

	::drmModeConnectorPtr handle_;
};




class Encoder {
public:
	static Encoder null() noexcept { return Encoder(nullptr); }

	static Encoder get(int fd, uint32_t id);

	~Encoder() noexcept;

	Encoder(Encoder && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	Encoder & operator=(Encoder && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::drmModeEncoderPtr() const noexcept { return handle_; }
	::drmModeEncoderPtr operator->() const noexcept { return handle_; }

private:
	Encoder(::drmModeEncoderPtr handle) noexcept :
		handle_(handle)
	{}

	::drmModeEncoderPtr handle_;
};




class Plane {
public:
	static Plane null() noexcept { return Plane(nullptr); }

	static Plane get(int fd, uint32_t id);

	~Plane() noexcept;

	Plane(Plane && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	Plane & operator=(Plane && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::drmModePlanePtr() const noexcept { return handle_; }
	::drmModePlanePtr operator->() const noexcept { return handle_; }

private:
	Plane(::drmModePlanePtr handle) noexcept :
		handle_(handle)
	{}

	::drmModePlanePtr handle_;
};




class PlaneRes {
public:
	static PlaneRes null() noexcept { return PlaneRes(nullptr); }

	static PlaneRes get(int fd);

	~PlaneRes() noexcept;

	PlaneRes(PlaneRes && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	PlaneRes & operator=(PlaneRes && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::drmModePlaneResPtr() const noexcept { return handle_; }
	::drmModePlaneResPtr operator->() const noexcept { return handle_; }

private:
	PlaneRes(::drmModePlaneResPtr handle) noexcept :
		handle_(handle)
	{}

	::drmModePlaneResPtr handle_;
};




class ObjectProperties {
public:
	static ObjectProperties null() noexcept { return ObjectProperties(nullptr); }

	static ObjectProperties get(int fd, uint32_t id, uint32_t type);

	~ObjectProperties() noexcept;

	ObjectProperties(ObjectProperties && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	ObjectProperties & operator=(ObjectProperties && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::drmModeObjectPropertiesPtr() const noexcept { return handle_; }
	::drmModeObjectPropertiesPtr operator->() const noexcept { return handle_; }

private:
	ObjectProperties(::drmModeObjectPropertiesPtr handle) noexcept :
		handle_(handle)
	{}

	::drmModeObjectPropertiesPtr handle_;
};




class Property {
public:
	static Property null() noexcept { return Property(nullptr); }

	static Property get(int fd, uint32_t prop);

	~Property() noexcept;

	Property(Property && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	Property & operator=(Property && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::drmModePropertyPtr() const noexcept { return handle_; }
	::drmModePropertyPtr operator->() const noexcept { return handle_; }

private:
	Property(::drmModePropertyPtr handle) noexcept :
		handle_(handle)
	{}

	::drmModePropertyPtr handle_;
};




class Crtc {
public:
	static Crtc null() noexcept { return Crtc(nullptr); }

	static Crtc get(int fd, uint32_t id);

	~Crtc() noexcept;

	Crtc(Crtc && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	Crtc & operator=(Crtc && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::drmModeCrtcPtr() const noexcept { return handle_; }
	::drmModeCrtcPtr operator->() const noexcept { return handle_; }

private:
	Crtc(::drmModeCrtcPtr handle) noexcept :
		handle_(handle)
	{}

	::drmModeCrtcPtr handle_;
};




extern Fd openDevice(const std::string_view & path = {});




inline Buffer::Buffer(Buffer && other) noexcept :
	devFd_(other.devFd_),
	handle_(std::move(other.handle_)),
	dmaBuffer_(std::move(other.dmaBuffer_))
{}


inline Buffer & Buffer::operator=(Buffer && other) noexcept
{
	devFd_ = other.devFd_;
	std::swap(handle_, other.handle_);
	std::swap(dmaBuffer_, other.dmaBuffer_);
	return *this;
}




} // Drm
