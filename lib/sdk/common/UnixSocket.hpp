
#pragma once

#include <functional>
#include <string_view>

#include "Fd.hpp"




class Socket {
public:
	static Socket connect(const std::string_view & socketFilePath);

	Socket() noexcept;
	~Socket() noexcept;

	Socket & operator=(Socket && other) noexcept;

	int fd() const noexcept { return socketFd_; }
	bool isNull() const noexcept { return socketFd_.isNull(); }

	template <typename T> void sendBlocking(const T & value) const { _sendBlocking(&value, sizeof(T)); }
	template <typename T> T recvBlocking() const { T value; _recvBlocking(&value, sizeof(T)); return value; }

	template <typename T> T peek() const { T value; _peek(&value, sizeof(T)); return value; }

	void readAll(const std::function<void()> & cb) const;

	void sendFd(int fd) const;
	int recvFd() const;

private:
	Socket(Fd && fd) noexcept;

	void _sendBlocking(const void * data, int size) const;
	void _recvBlocking(void * data, int size) const;

	void _peek(void * data, int size) const;

	Fd socketFd_;

	friend class SocketServer;
};




class SocketServer {
public:
	static SocketServer null() { return SocketServer(Fd::null()); }
	static SocketServer listen(const std::string_view & socketFilePath, int maxConnections);

	~SocketServer() noexcept;

	int fd() const noexcept { return serverFd_; }
	bool isNull() const noexcept { return serverFd_.isNull(); }

	Socket accept() const;

private:
	SocketServer(Fd && fd) noexcept;

	Fd serverFd_;
};
