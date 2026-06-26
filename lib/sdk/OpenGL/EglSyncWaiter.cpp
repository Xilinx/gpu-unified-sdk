/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "EglSyncWaiter.hpp"

#include "EGLUtils.hpp"
#include "Log.hpp"




EglSyncWaiter::EglSyncWaiter(const EGLDisplay display) :
	display_(display)
{
	thread_ = std::thread([this] {
		try {
			_exec();
		} catch (const Exception & e) {
			ERROR("Caught Exception:")
			e.print();
		}
	});
}


EglSyncWaiter::~EglSyncWaiter() noexcept
{
	ASSERT(thread_.joinable())
	{
		std::lock_guard<std::mutex> lock(mutex_);
		quit_ = true;
		condition_.notify_one();
	}
	thread_.join();
}


void EglSyncWaiter::add(const EGLSyncKHR sync, const DoneCallback & cb) noexcept
{
	std::lock_guard<std::mutex> lock(mutex_);
	items_.push(Item{ .sync = sync, .cb = cb });
	condition_.notify_one();
}


void EglSyncWaiter::_exec()
{
	Log::setThreadLocalColor(Log::Color::Magenta);

	while (true) {
		auto maybeItem = [this] () -> std::optional<Item> {
			std::unique_lock<std::mutex> lock(mutex_);
			condition_.wait(lock, [this] { return quit_ || !items_.empty(); });
			if (quit_) return {};
			Item item = items_.front();
			items_.pop();
			return item;
		}();

		if (!maybeItem.has_value()) break;

		Item & item = *maybeItem;

		GL_DYNAMIC_FUNCTIONS_NAMESPACE eglClientWaitSyncKHR(display_, item.sync, 0, EGL_FOREVER_KHR);

		item.cb();
	}
}
