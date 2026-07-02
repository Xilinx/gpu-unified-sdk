/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <queue>

#include <EGL/egl.h>
#include <EGL/eglext.h>




class EglSyncWaiter {
public:
	using DoneCallback = std::function<void()>;

	EglSyncWaiter(EGLDisplay display);
	~EglSyncWaiter() noexcept;

	void add(EGLSyncKHR sync, const DoneCallback & cb) noexcept;

private:
	struct Item {
		EGLSyncKHR sync;
		DoneCallback cb;
	};

	void _exec();

	const EGLDisplay display_;

	std::mutex mutex_;
	std::condition_variable condition_;
	std::queue<Item> items_;
	bool quit_ = false;
	std::thread thread_;
};
