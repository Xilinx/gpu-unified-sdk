/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "FrameCounter.hpp"




FrameCounter::FrameCounter(const int64_t maxTimeNsecs) noexcept :
	maxTimeNsecs_(maxTimeNsecs)
{
}


double FrameCounter::fps() const noexcept
{
	return fps_;
}


void FrameCounter::reset() noexcept
{
	totalCount_ = 0;
	totalDurationNsecs_ = 0;
	totalTimeNsecs_ = 0;
	fps_ = 0.0;
	segmentsPos_ = 0;
	segmentsSize_ = 0;
}


void FrameCounter::add(const int count, const int64_t durationNsecs,
		const int64_t timeNsecs) noexcept
{
	if (segmentsSize_ == kMaxSegmentCount) {
		_pop();
	}

	_push(Segment {
		.count = count,
		.durationNsecs = durationNsecs,
		.timeNsecs = timeNsecs == -1 ? durationNsecs : timeNsecs,
	});

	_calc();
}


void FrameCounter::_push(const Segment & s) noexcept
{
	const int pos = (segmentsPos_ + segmentsSize_) % kMaxSegmentCount;
	segments_[pos] = s;

	totalCount_ += s.count;
	totalDurationNsecs_ += s.durationNsecs;
	totalTimeNsecs_ += s.timeNsecs;

	segmentsSize_++;
}


void FrameCounter::_pop() noexcept
{
	const Segment & s = segments_[segmentsPos_];

	totalCount_ -= s.count;
	totalDurationNsecs_ -= s.durationNsecs;
	totalTimeNsecs_ -= s.timeNsecs;

	segmentsSize_--;
	segmentsPos_ = (segmentsPos_ + 1) % kMaxSegmentCount;
}


void FrameCounter::_calc() noexcept
{
	if (segmentsSize_ == 0) return;

	while (totalTimeNsecs_ > maxTimeNsecs_) {
		_pop();
	}

	if (totalDurationNsecs_ == 0) {
		fps_ = 999'999.0;
	} else {
		fps_ = static_cast<double>(totalCount_) * 1'000'000'000.0 / double(totalDurationNsecs_);
	}
}
