/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <array>

#include <fcntl.h>

#include "Fd.hpp"




class Pipe {
public:
	static Pipe null() noexcept { return Pipe(_nullPair()); }
	static Pipe create() { return Pipe(_createPair()); }

	~Pipe() noexcept = default;

	const Fd & read() const noexcept { return fds_[0]; }
	Fd & read() noexcept { return fds_[0]; }

	const Fd & write() const noexcept { return fds_[1]; }
	Fd & write() noexcept { return fds_[1]; }

private:
	using Pair = std::array<Fd, 2>;

	static Pair _nullPair() noexcept;
	static Pair _createPair();

	Pipe(Pair && pair) noexcept;

	Pair fds_;
};
