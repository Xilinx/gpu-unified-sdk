/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "TextReader.hpp"

#include <cstring>




static inline constexpr int kSizeIncrese = 16;




TextReader::TextReader(std::istream & stream) noexcept :
	stream_(stream)
{
	bufferSize_ = kSizeIncrese;
	buffer_.reset(new char[bufferSize_ + 1]);
}


bool TextReader::next() noexcept
{
	int lineSize = 0;

	while (true) {
		if (lineSize == bufferSize_) {
			const int newSize = lineSize + kSizeIncrese;
			std::unique_ptr<char[]> newBuffer(new char[newSize + 1]);
			std::memcpy(newBuffer.get(), buffer_.get(), lineSize + 1);
			buffer_ = std::move(newBuffer);
			bufferSize_ = newSize;
		}

		stream_.getline(buffer_.get() + lineSize, kSizeIncrese + 1);

		if (!stream_.fail()) {
			const int length = std::strlen(buffer_.get() + lineSize);
			lineSize_ = lineSize + length;
			break;
		}

		if (stream_.eof()) {
			return false;
		}

		stream_.clear();
		lineSize += kSizeIncrese;
	}

	lineNumber_++;

	return true;
}
