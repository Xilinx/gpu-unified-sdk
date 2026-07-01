/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <array>
#include <cstdint>




class FrameCounter {
public:
	inline static const int kMaxSegmentCount = 1000;

	FrameCounter(int64_t maxTimeNsecs) noexcept;

	double fps() const noexcept;

	int64_t maxTime() const noexcept;

	void reset() noexcept;

	void add(int count, int64_t durationNsecs, int64_t timeoutUsecs = -1) noexcept;

private:
	struct Segment {
		int count;
		int64_t durationNsecs;
		int64_t timeNsecs;
	};

	void _push(const Segment & s) noexcept;
	void _pop() noexcept;
	void _calc() noexcept;

	const int64_t maxTimeNsecs_;

	std::array<Segment, kMaxSegmentCount> segments_;
	int segmentsPos_ = 0;
	int segmentsSize_ = 0;
	int totalCount_ = 0;
	int64_t totalDurationNsecs_ = 0;
	int64_t totalTimeNsecs_ = 0;
	double fps_ = 0.0;
};




inline int64_t FrameCounter::maxTime() const noexcept
{
	return maxTimeNsecs_;
}
