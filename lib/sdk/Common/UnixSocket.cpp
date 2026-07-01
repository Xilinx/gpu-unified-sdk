/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "UnixSocket.hpp"

#include <cerrno>
#include <cstring>
#include <cstdlib>

#include <sys/socket.h>
#include <sys/un.h>

#include "Log.hpp"




Socket Socket::connect(const std::string_view & socketFilePath)
{
	CHECK(socketFilePath.size() < std::extent_v<decltype(::sockaddr_un::sun_path)>,
			"Socket file path too long:", socketFilePath)

	Fd fd = Fd::adapt(::socket(AF_UNIX, SOCK_STREAM, 0));
	CHECK(!fd.isNull())

	::sockaddr_un remote;
	remote.sun_family = AF_UNIX;
	std::strncpy(remote.sun_path, socketFilePath.data(), sizeof(remote.sun_path) - 1);

	const ::socklen_t len = socketFilePath.size() + sizeof(remote.sun_family);
	const int connectRet = ::connect(fd, (struct sockaddr*)&remote, len);
	CHECK(connectRet != -1, "Failed to connect socket:", socketFilePath, "ret:", connectRet)

	return Socket(std::move(fd));
}


Socket::Socket() noexcept :
	socketFd_(Fd::null())
{
}


Socket & Socket::operator=(Socket && other) noexcept
{
	socketFd_ = std::move(other.socketFd_);
	return *this;
}


Socket::Socket(Fd && fd) noexcept :
	socketFd_(std::move(fd))
{
}


Socket::~Socket() noexcept
{
}


void Socket::readAll(const std::function<void()> & cb) const
{
	while (true) {
		uint8_t byte;
		const int recvSize = ::recv(socketFd_, &byte, 1, MSG_PEEK | MSG_DONTWAIT);
		if (recvSize == 1) {
			cb();
		} else {
			const int err = errno;
			CHECK(err == EAGAIN, Log::attach(err), "err:", err, std::strerror(err))
			break;
		}
	}
}


int Socket::recvFd() const
{
	constexpr int kCmsgSize = sizeof(::cmsghdr) + sizeof(int);
	std::array<uint8_t, kCmsgSize> cmsgBuffer;
	::cmsghdr * const cmsg = reinterpret_cast<::cmsghdr*>(cmsgBuffer.data());

	char iovbuf[64];
	::iovec iov {
		.iov_base = iovbuf,
		.iov_len = sizeof(iovbuf),
	};

	::msghdr msg {
		.msg_name = nullptr,
		.msg_namelen = 0,
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = (caddr_t)cmsg,
		.msg_controllen = kCmsgSize,
	};

	const int ret = ::recvmsg(socketFd_, &msg, 0);
	CHECK(ret >= 0, "errno:", errno)

	return *(int *)CMSG_DATA(cmsg);
}


void Socket::sendFd(const int fd) const
{
	constexpr int kCmsgBytes = sizeof(int);

	char cmsgbuf[CMSG_SPACE(kCmsgBytes)];
	std::memset(cmsgbuf, '\0', sizeof(cmsgbuf));

	char iovbuf[64];
	::iovec iov = {
		.iov_base = iovbuf,
		.iov_len = sizeof(iovbuf),
	};

	::msghdr msg {
		.msg_name = nullptr,
		.msg_namelen = 0,
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = cmsgbuf,
		.msg_controllen = CMSG_SPACE(kCmsgBytes),
	};

	struct cmsghdr * cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(kCmsgBytes);

	memcpy(CMSG_DATA(cmsg), &fd, kCmsgBytes);

	const int ret = ::sendmsg(socketFd_, &msg, 0);
	CHECK(ret >= 0, "errno:", errno)
}


void Socket::_sendBlocking(const void * const data, const int size) const
{
	const int sentSize = ::send(socketFd_, data, size, MSG_WAITALL);
	CHECK(sentSize == size)
}


void Socket::_recvBlocking(void * const data, const int size) const
{
	const int recvSize = ::recv(socketFd_, data, size, MSG_WAITALL);
	CHECK(recvSize == size, recvSize, size)
}


void Socket::_peek(void * const data, const int size) const
{
	const int recvSize = ::recv(socketFd_, data, size, MSG_PEEK | MSG_DONTWAIT);
	CHECK(recvSize == size, Log::attach(int(errno)))
}




SocketServer::SocketServer(Fd && fd) noexcept :
	serverFd_(std::move(fd))
{
}


SocketServer::~SocketServer() noexcept
{
}


SocketServer SocketServer::listen(const std::string_view & socketFilePath, const int maxConnections)
{
	CHECK(socketFilePath.size() < std::extent_v<decltype(::sockaddr_un::sun_path)>,
			"Socket file path too long:", socketFilePath)

	Fd fd = Fd::adapt(::socket(AF_UNIX, SOCK_STREAM, 0));
	CHECK(!fd.isNull())

	::sockaddr_un local;
	local.sun_family = AF_UNIX;
	std::strncpy(local.sun_path, socketFilePath.data(), sizeof(local.sun_path) - 1);

	const ::socklen_t len = socketFilePath.size() + sizeof(local.sun_family);
	const int bindRet = ::bind(fd, (struct sockaddr*)&local, len);
	CHECK(bindRet == 0, "Failed to bind socket:", socketFilePath, "ret:", bindRet)

	const int listenRet = ::listen(fd, maxConnections);
	CHECK(listenRet == 0, "Failed to listen socket:", socketFilePath, "ret:", listenRet)

	return SocketServer(std::move(fd));
}


Socket SocketServer::accept() const
{
	::sockaddr_un remote;
	::socklen_t len;
	Fd fd = Fd::adapt(::accept(serverFd_, (struct sockaddr*)&remote, &len));
	CHECK(!fd.isNull(), "Failed to accept socket")
	return Socket(std::move(fd));
}
