
#pragma once

#include <istream>
#include <memory>




class TextReader {
public:
	TextReader(std::istream & stream) noexcept;

	int lineNumber() const noexcept { return lineNumber_; }
	bool atEnd() const noexcept { return atEnd_; }
	std::string_view line() const noexcept { return std::string_view(buffer_.get(), lineSize_); }

	bool next() noexcept;

private:
	std::istream & stream_;
	int lineNumber_ = 0;
	bool atEnd_ = false;
	int bufferSize_ = 0;
	int lineSize_ = 0;
	std::unique_ptr<char[]> buffer_;
};
