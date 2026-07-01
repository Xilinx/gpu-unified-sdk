/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "SyncCallDebugger.hpp"

#include <Common/Log.hpp>




SyncCallDebugger::SyncCallDebugger() noexcept
{
	thread_ = std::thread([this] {
		_thread();
	});
}


SyncCallDebugger::~SyncCallDebugger() noexcept
{
	std::lock_guard lock(mutex_);
	quit_ = true;
	waiter_.notify_one();
	thread_.join();
}


void SyncCallDebugger::add(const std::string_view & name, const int64_t ns) noexcept
{
	ASSERT(!name.empty())
	ASSERT(ns > 0)
	std::lock_guard lock(mutex_);
	_reset();
	addTime_ = std::chrono::steady_clock::now();
	name_ = name;
	ns_ = ns;
	waiter_.notify_one();
}


void SyncCallDebugger::reset() noexcept
{
	std::lock_guard lock(mutex_);
	_reset();
	waiter_.notify_one();
}


void SyncCallDebugger::_thread() noexcept
{
	while (true) {
		std::unique_lock lock(mutex_);
		const auto ok = [this, &lock] () -> bool {
			if (name_.empty()) {
				waiter_.wait(lock);
				return true;
			} else {
				return waiter_.wait_for(lock, std::chrono::nanoseconds(ns_)) != std::cv_status::timeout;
			}
		}();
		if (quit_) break;
		if (!name_.empty()) {
			if (!ok) {
				if (!hasError_) {
					hasError_ = true;
					const auto duration = std::chrono::steady_clock::now() - addTime_;
					DEBUG(Log::braced(name_), "not responding for:",
							double(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()) / 1'000'000, "ms")
				}
			}
		}
	}
}


void SyncCallDebugger::_reset() noexcept
{
	if (name_.empty()) {
		return;
	}

	if (hasError_) {
		hasError_ = false;
		const auto duration = std::chrono::steady_clock::now() - addTime_;
		DEBUG(Log::braced(name_), "responded after:",
				double(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()) / 1'000'000, "ms")
	}

	name_ = {};
}
