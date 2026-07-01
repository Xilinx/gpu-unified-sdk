/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "Common/Log.hpp"




namespace av {




namespace detail {
	struct FormatContextDeleter {
		void operator()(::AVFormatContext * c) noexcept {
			::avformat_free_context(c);
		}
	};

	struct CodecContextDeleter {
		void operator()(::AVCodecContext * c) noexcept {
			::avcodec_free_context(&c);
		}
	};

	struct IoContextDeleter {
		void operator()(::AVIOContext * io) noexcept {
			::avio_closep(&io);
		}
	};

	struct FrameDeleter {
		void operator()(::AVFrame * frame) noexcept {
			::av_frame_free(&frame);
		}
	};
}

using FormatContext = std::unique_ptr<::AVFormatContext, detail::FormatContextDeleter>;
using CodecContext = std::unique_ptr<::AVCodecContext, detail::CodecContextDeleter>;
using IoContext = std::unique_ptr<::AVIOContext, detail::IoContextDeleter>;
using Frame = std::unique_ptr<::AVFrame, detail::FrameDeleter>;

struct Error {
	Error(int err) noexcept : err(err) {}
	int  err;
};

struct CodecId {
	CodecId(AVCodecID id) noexcept : id(id) {}
	AVCodecID id;
};




} // av




template <>
struct Log::Write<av::Error> {
	static void write(std::ostream & s, const av::Error & e) noexcept
	{
		char buf[AV_ERROR_MAX_STRING_SIZE];
		::av_strerror(e.err, buf, sizeof(buf));
		s << Log::format("AVERROR(%d %s)", e.err, buf);
	}
};


template <>
struct Log::Write<av::CodecId> {
	static void write(std::ostream & s, const av::CodecId & c) noexcept
	{
		s << Log::format("AVCodecID(%s)", ::avcodec_get_name(c.id));
	}
};
