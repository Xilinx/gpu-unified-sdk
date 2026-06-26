/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "Pipe.hpp"

#include "Log.hpp"




Pipe::Pipe(Pair && pair) noexcept :
	fds_(std::move(pair))
{
}


Pipe::Pair Pipe::_nullPair() noexcept
{
	return Pair{
		Fd::null(),
		Fd::null(),
	};
}


std::array<Fd, 2> Pipe::_createPair()
{
	int fds[2];
	const int ret = ::pipe2(fds, O_NONBLOCK);
	CHECK(ret != -1)
	return Pair{
		Fd::adapt(fds[0]),
		Fd::adapt(fds[1]),
	};
}
