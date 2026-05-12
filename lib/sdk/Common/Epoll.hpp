
#pragma once

#include "Fd.hpp"

#include <sys/epoll.h>




class Epoll {
public:
	static Epoll null() noexcept { return Epoll(Fd::null()); }

	static Epoll create()
	{
		Fd fd = Fd::adapt(::epoll_create1(0));
		CHECK(!fd.isNull(), Log::err())
		return Epoll(std::move(fd));
	}

	void add(const int fd, const int events) const
	{
		::epoll_event ev;
		ev.events = events;
		ev.data.fd = fd;
		const int ret = ::epoll_ctl(fd_, EPOLL_CTL_ADD, fd, &ev);
		CHECK(ret == 0, Log::err())
	}

	void remove(int fd) const
	{
		::epoll_event ev;
		ev.events = 0;
		ev.data.fd = fd;
		const int ret = ::epoll_ctl(fd_, EPOLL_CTL_DEL, fd, &ev);
		CHECK(ret == 0, Log::err())
	}

	void mod(const int fd, const int events) const
	{
		::epoll_event ev;
		ev.events = events;
		ev.data.fd = fd;
		const int ret = ::epoll_ctl(fd_, EPOLL_CTL_MOD, fd, &ev);
		CHECK(ret == 0, Log::err())
	}

	const Fd & fd() const noexcept { return fd_; }
	operator int() const noexcept { return fd_; }

private:
	Epoll(Fd && fd) noexcept : fd_(std::move(fd)) {}

	Fd fd_;
};
