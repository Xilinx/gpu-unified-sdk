/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <chrono>
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <string_view>
#include <thread>




class SyncCallDebugger {
public:
	SyncCallDebugger() noexcept;
	~SyncCallDebugger() noexcept;

	void add(const std::string_view & name, int64_t ns) noexcept;
	void reset() noexcept;

private:
	void _thread() noexcept;
	void _reset() noexcept;

	std::mutex mutex_;
	std::condition_variable waiter_;
	std::thread thread_;
	std::string_view name_;
	int64_t ns_ = 0;
	std::chrono::steady_clock::time_point addTime_;
	bool hasError_ = false;
	bool quit_ = false;
};
