
#pragma once

#include <utility>

#include <unistd.h>

#include "Log.hpp"




class Fd {
public:
	inline static constexpr int kNullFd = -1;

	static Fd null() noexcept
	{
		return Fd(kNullFd, false);
	}

	static Fd adapt(int fd) noexcept
	{
		return Fd(fd, false);
	}

	static Fd dup(int fd)
	{
		const int dupFd = ::dup(fd);
		CHECK(dupFd != kNullFd, Log::err())
		return Fd::adapt(dupFd);
	}

	static Fd weak(int fd) noexcept
	{
		return Fd(fd, true);
	}

	static int readAll(int fd) noexcept
	{
		int totalSize = 0;
		while (true) {
			uint32_t v;
			const int readSize = ::read(fd, &v, sizeof(v));
			if (readSize >= 0) totalSize += readSize;
			if (readSize != sizeof(v)) break;
		}
		return totalSize;
	}

	~Fd() noexcept
	{
		if (fd_ != kNullFd && !weak_) {
			::close(fd_);
		}
	}

	Fd(Fd && other) noexcept :
		fd_(std::exchange(other.fd_, kNullFd)),
		weak_(other.weak_)
	{
	}

	Fd & operator=(Fd && other) noexcept
	{
		std::swap(fd_, other.fd_);
		std::swap(weak_, other.weak_);
		return *this;
	}

	int fd() const noexcept { return fd_; }
	operator int() const noexcept { return fd_; }

	bool isNull() const noexcept { return fd_ == kNullFd; }
	int release() noexcept { return std::exchange(fd_, kNullFd); }

	Fd dup() const
	{
		CHECK(!isNull())
		const int dupFd = ::dup(fd_);
		CHECK(dupFd != kNullFd, Log::err())
		return Fd::adapt(dupFd);
	}

	template <typename T>
	void write(const T & value) const
	{
		write(&value, sizeof(T));
	}

	template <typename T>
	T read() const
	{
		T value;
		read(&value, sizeof(T));
		return value;
	}

	void write(const void * const data, const int size) const
	{
		const int writtenSize = ::write(fd_, data, size);
		CHECK(writtenSize == size)
	}

	void read(void * const data, const int size) const
	{
		const int readSize = ::read(fd_, data, size);
		CHECK(readSize == size, Log::attach(int(errno)))
	}

	Fd(const Fd &) = delete;
	void operator=(const Fd &) = delete;

private:
	Fd(int fd, bool weak) noexcept :
		fd_(fd),
		weak_(weak)
	{
	}

private:
	int fd_;
	bool weak_;
};
