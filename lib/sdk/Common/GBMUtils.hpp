/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <utility>

#include <gbm.h>

#include <Common/Geometry.hpp>




namespace Gbm {




class Device {
public:
	static Device get(int fd);

	~Device() noexcept;

	Device(Device && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	Device & operator=(Device && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::gbm_device*() const noexcept { return handle_; }

private:
	Device(::gbm_device * handle) noexcept :
		handle_(handle)
	{}

	::gbm_device * handle_;
};




class Surface {
public:
	static Surface create(const Device & device, const Size & size, uint32_t format,
			const uint64_t * modifiers, int modifierCount, uint32_t flags);

	~Surface() noexcept;

	Surface(Surface && other) noexcept :
		handle_(std::exchange(other.handle_, nullptr))
	{}

	Surface & operator=(Surface && other) noexcept
	{
		std::swap(handle_, other.handle_);
		return *this;
	}

	operator ::gbm_surface*() const noexcept { return handle_; }

private:
	Surface(::gbm_surface * handle) noexcept :
		handle_(handle)
	{}

	::gbm_surface * handle_;
};




} // Gbm
