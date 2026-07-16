/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <utility>




namespace HandleWrapper {
namespace detail {




template <typename T>
class Wrapper;


template <typename T>
struct Traits;


template <typename T>
class Base {
public:
	const Wrapper<T> & cast() const
	{
		return static_cast<const Wrapper<T> &>(*this);
	}

	Wrapper<T> & cast()
	{
		return static_cast<Wrapper<T> &>(*this);
	}
};


template <typename T>
class Handle;


template <typename T>
class Wrapper : public Handle<T> {
public:
	using Type = T;
	using H = Handle<T>;
	using W = Wrapper<T>;
	using Data = H::Data;
	inline static constexpr T Null = Traits<T>::NullValue;

	static W null() noexcept {
		return W(Data{}, Null);
	}

	static W adapt(const Data & data, const T & handle) noexcept
	{
		return W(data, handle);
	}

	~Wrapper() noexcept
	{
		if (handle_ != Null) {
			H::destroy(data_, handle_);
		}
	}

	const Data & data() const noexcept { return data_; }
	const T & handle() const noexcept { return handle_; }
	operator const T &() const noexcept { return handle_; }
	operator bool() const noexcept { return handle_ != Null; }

	Wrapper(W && other) noexcept :
		data_(other.data_),
		handle_(std::exchange(other.handle_, Null))
	{}

	W & operator=(W && other) noexcept
	{
		if (handle_ != Null) {
			H::destroy(data_, handle_);
		}
		data_ = other.data_;
		handle_ = std::exchange(other.handle_, Null);
		return *this;
	}

	Wrapper() = delete;
	Wrapper(const Wrapper &) = delete;
	Wrapper & operator=(const Wrapper &) = delete;

private:
	explicit Wrapper(const Data & data, const T & handle) noexcept :
		data_(data),
		handle_(handle)
	{}

	Data data_;
	T handle_;
};




}} // HandleWrapper::detail
