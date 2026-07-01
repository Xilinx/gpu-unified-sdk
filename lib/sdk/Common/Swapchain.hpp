/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <vector>




class Swapchain {
public:
	enum class Mode {
		Mailbox,
		Fifo,
	};

	enum class State {
		Invalid     = -1,
		Renderering = 0,
		Ready       = 1,
		Presenting  = 2,
		Free        = 3,
	};

	Swapchain(Mode mode, int maxRenderingCount, int maxPresentingCount);
	~Swapchain() noexcept;

	int bufferCount() const noexcept { return buffers_.size(); }
	State bufferState(int index) const noexcept { return buffers_[index].state; }
	State previousBufferState(int index) const noexcept { return buffers_[index].previousState; }

	int maxRenderingBufferCount() const noexcept { return maxRenderingCount_; }
	int maxPresentingBufferCount() const noexcept { return maxPresentingCount_; }

	bool hasBufferToRender() const noexcept;
	int makeRenderingBuffer() noexcept;
	void makeRenderingBuffer(int index) noexcept;
	void releaseRenderingBuffer(int index) noexcept;

	int readyBufferCount() const noexcept { return _list(State::Ready).count; }
	int presentingBufferCount() const noexcept { return _list(State::Presenting).count; }
	int makePresentingBuffer() noexcept;
	void makePresentingBuffer(int index) noexcept;
	void releasePresentingBuffer(int index) noexcept;

private:
	struct Buffer {
		State previousState;
		State state;
		int previous;
		int next;
	};

	struct List {
		int size = -1;
		int first = -1;
		int last = -1;
		int count = 0;
	};

	const List & _list(State state) const noexcept { return lists_[int(state)]; }
	List & _list(State state) noexcept { return lists_[int(state)]; }

	void _dump() const noexcept;

	void _take(int index) noexcept;
	void _put(int index, State state, bool first, State previousState) noexcept;
	void _move(int index, State state, bool first) noexcept;

	int _nextBufferToRender() const noexcept;

	const Mode mode_;
	const int maxRenderingCount_;
	const int maxPresentingCount_;

	std::vector<Buffer> buffers_;
	List lists_[4];
};
