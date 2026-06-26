/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "Swapchain.hpp"

#include "Log.hpp"




Swapchain::Swapchain(const Mode mode, const int maxRenderingCount, const int maxPresentingCount) :
	mode_(mode),
	maxRenderingCount_(maxRenderingCount),
	maxPresentingCount_(maxPresentingCount)
{
	buffers_.resize(maxRenderingCount + maxPresentingCount);

	for (int bufferIndex = 0; bufferIndex < int(buffers_.size()); ++bufferIndex) {
		Buffer & buffer = buffers_.at(bufferIndex);
		buffer.previousState = State::Invalid;
		buffer.state = State::Free;
		buffer.previous = bufferIndex - 1;
		buffer.next = bufferIndex + 1;
	}
	buffers_.front().previous = -1;
	buffers_.back().next = -1;

	List & freeList = _list(State::Free);
	freeList.count = buffers_.size();
	freeList.first = 0;
	freeList.last = buffers_.size() - 1;

	_list(State::Presenting).size = maxPresentingCount;
}


Swapchain::~Swapchain() noexcept
{
}


void Swapchain::_dump() const noexcept
{
	DEBUG("buffers:")
	for (int index = 0; index < int(buffers_.size()); ++index) {
		const Buffer & buffer = buffers_[index];
		DEBUG("    index:", index, "state:", int(buffer.state), buffer.previous, buffer.next)
	}
	DEBUG("lists:")
	for (int i = 0; i < 4; ++i) {
		const List & l = _list(State(i));
		DEBUG("    state:", i, "count:", l.count, "first:", l.first, "last:", l.last)
	}
}


void Swapchain::_take(const int index) noexcept
{
	Buffer & buffer = buffers_[index];
	ASSERT(buffer.state != State::Invalid)

	List & l = _list(buffer.state);
	ASSERT(l.count != 0)
	ASSERT(l.first != -1)
	ASSERT(l.last != -1)

	l.count--;

	if (buffer.previous != -1) {
		buffers_[buffer.previous].next = buffer.next;
	} else {
		ASSERT(l.first == index)
		l.first = buffer.next;
	}
	if (buffer.next != -1) {
		buffers_[buffer.next].previous = buffer.previous;
	} else {
		ASSERT(l.last == index)
		l.last = buffer.previous;
	}

	buffer.state = State::Invalid;
}


void Swapchain::_put(const int index, const State state, const bool first,
		const State previousState) noexcept
{
	Buffer & buffer = buffers_[index];
	ASSERT(buffer.state == State::Invalid)

	List & l = _list(state);
	ASSERT(l.size == -1 || l.count < l.size)

	l.count++;

	buffer.state = state;
	buffer.previousState = previousState;

	if (first) {
		if (l.first != -1) {
			buffers_[l.first].previous = index;
		}
		buffer.next = l.first;
		buffer.previous = -1;
		l.first = index;
		if (l.last == -1) {
			l.last = index;
		}
	} else {
		if (l.last != -1) {
			buffers_[l.last].next = index;
		}
		buffer.previous = l.last;
		buffer.next = -1;
		l.last = index;
		if (l.first == -1) {
			l.first = index;
		}
	}
}


void Swapchain::_move(const int index, const State state, const bool first) noexcept
{
	const State previousState = buffers_[index].state;
	_take(index);
	_put(index, state, first, previousState);
}


int Swapchain::_nextBufferToRender() const noexcept
{
	switch (mode_) {
	case Mode::Fifo: {
		// rendering buffers may be used only from free queue
		return _list(State::Free).first;
	}
	case Mode::Mailbox: {
		// rendering buffers may be used also from ready queue
		const int freeIndex = _list(State::Free).first;
		if (freeIndex != -1) {
			return freeIndex;
		} else {
			const List & l = _list(State::Ready);
			if (l.count < 2) {
				return -1;
			} else {
				return l.first;
			}
		}
	}
	default: [[unlikely]]
		FATAL()
	}
}


bool Swapchain::hasBufferToRender() const noexcept
{
	return _nextBufferToRender() != -1;
}


int Swapchain::makeRenderingBuffer() noexcept
{
	const int index = _nextBufferToRender();
	ASSERT(index != -1)
	_move(index, State::Renderering, true);
	return index;
}


void Swapchain::makeRenderingBuffer(const int index) noexcept
{
	ASSERT(index > 0 && index < buffers_.size())
	const Buffer & buffer = buffers_[index];
	ASSERT(buffer.state == State::Free || buffer.state == State::Ready)
	_move(index, State::Renderering, true);
}


void Swapchain::releaseRenderingBuffer(const int index) noexcept
{
	Buffer & buffer = buffers_[index];
	ASSERT(buffer.state == State::Renderering)
	_move(index, State::Ready, false);
}


int Swapchain::makePresentingBuffer() noexcept
{
	const List & l = _list(State::Presenting);
	ASSERT(l.count < l.size);
	const int index = _list(State::Ready).first;
	ASSERT(index != -1)
	_move(index, State::Presenting, true);
	return index;
}


void Swapchain::makePresentingBuffer(const int index) noexcept
{
	ASSERT(index > 0 && index < buffers_.size())
	const Buffer & buffer = buffers_[index];
	ASSERT(buffer.state == State::Ready)
	_move(index, State::Presenting, true);
}


void Swapchain::releasePresentingBuffer(const int index) noexcept
{
	Buffer & buffer = buffers_.at(index);
	ASSERT(buffer.state == State::Presenting)
	_move(index, State::Free, true);
}
