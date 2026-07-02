/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <cstdint>
#include <utility>

#include <sys/mman.h>




template <typename T>
class Mapped {
public:
	static Mapped null() noexcept { return Mapped(nullptr, 0); }
	static Mapped adapt(void * data, int64_t size) noexcept { return Mapped(data, size); }

	~Mapped() noexcept
	{
		if (data_) {
			::munmap(data_, size_);
		}
	}

	Mapped(Mapped && other) noexcept :
		data_(std::exchange(other.data_, nullptr)),
		size_(other.size_)
	{}

	Mapped & operator=(Mapped && other) noexcept
	{
		std::swap(data_, other.data_);
		size_ = other.size_;
		return *this;
	}

	Mapped(const Mapped &) = delete;
	void operator=(const Mapped &) = delete;

	T * data() const noexcept { return data_; }
	int64_t size() const noexcept { return size_; }

private:
	Mapped(void * data, int64_t size) noexcept :
		data_(reinterpret_cast<T*>(data)),
		size_(size)
	{}

	T * data_;
	int64_t size_;
};
