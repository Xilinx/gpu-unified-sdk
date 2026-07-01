/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "Log.hpp"

#include <cstring>

#include <unistd.h>

#ifdef WIN32
#include <processthreadsapi.h>
#endif




static constexpr int kFileAlignment =
#ifdef LOG_FILE_ALIGNMENT
		LOG_FILE_ALIGNMENT
#else
		48;
#endif

Log::Global Log::global_;


struct ThreadLocal {
	Log::Color color = Log::Color::Null;
	int indentSize = 0;
	std::array<char, 64> indentBuffer;
	std::ostream * outputStream = nullptr;
	std::ostream * errorStream = nullptr;
};

thread_local ThreadLocal tl;


static std::ostream & streamForType(const Log::Stream::Type type) noexcept
{
	switch (type) {
	case Log::Stream::Type::Output:
		return tl.outputStream ? *tl.outputStream : std::cout;
	case Log::Stream::Type::Error:
		return tl.errorStream ? *tl.errorStream : std::cerr;
	default: [[unlikely]]
		FATAL()
	}
}


int Log::getCurrentThreadId() noexcept
{
#ifdef WIN32
	return ::GetCurrentThreadId();
#else
	return ::gettid();
#endif
}


void Log::init(const Init & init) noexcept
{
	[enableColors = init.enableColors] {
		if (enableColors.has_value()) {
			global_.enableColors = *enableColors;
			return;
		}

		if (const char * const useEnv = std::getenv("LOG_USE_COLORS")) {
			const auto use = std::string_view(useEnv);
			if (use == "always") {
				global_.enableColors = true;
				return;
			}
			if (use == "never") {
				global_.enableColors = false;
				return;
			}
		}

		const bool termSupportsColors = [] () -> bool {
			const char * const termEnv = std::getenv("TERM");
			if (!termEnv) return false;
			const std::string_view term = termEnv;
			if (term == "xterm-256color") return true;
			return false;
		}();

		global_.enableColors = termSupportsColors;
	}();

	[printContext = init.printContext] {
		if (printContext.has_value()) {
			global_.printContext = *printContext;
		}

		if (const char * const printContextEnv = std::getenv("LOG_PRINT_CONTEXT")) {
			const auto p = std::string_view(printContextEnv);
			if (p == "1") {
				global_.printContext = true;
				return;
			}
			if (p == "0") {
				global_.printContext = false;
				return;
			}
		}
	}();

	[level = init.level] {
		auto resolvedLevel = level;

		if (!resolvedLevel.has_value()) {
			if (const char * const levelEnv = std::getenv("LOG_LEVEL")) {
				resolvedLevel = [] (const std::string_view & name) -> std::optional<Level> {
					if (name == "none")  return Level::None;
					if (name == "error") return Level::Error;
					if (name == "info")  return Level::Info;
					if (name == "debug") return Level::Debug;
					return {};
				}(levelEnv);
			}
		}

		if (resolvedLevel.has_value()) {
			global_.level = *resolvedLevel;
			return;
		}
	}();
}


void Log::printErrno(const int err, std::ostream & ostream) noexcept
{
	ostream << err << " (" << ::strerror(err) << ")";
}


void Log::setThreadLocalColor(const Color color) noexcept
{
	tl.color = color;
}


Log::Indent Log::indent(const std::string_view & value) noexcept
{
	const int size = std::min(value.size(), tl.indentBuffer.size() - tl.indentSize);
	std::memcpy(tl.indentBuffer.data() + tl.indentSize, value.data(), size);
	tl.indentSize += size;
	return Indent(size);
}


std::string_view Log::completeIndent() noexcept
{
	return std::string_view(tl.indentBuffer.data(), tl.indentSize);
}


void Log::_unindent(const int size) noexcept
{
	tl.indentSize -= size;
}


Log::Stream Log::output(std::ostream & stream) noexcept
{
	return Stream(Stream::Type::Output, std::exchange(tl.outputStream, &stream));
}


Log::Stream Log::error(std::ostream & stream) noexcept
{
	return Stream(Stream::Type::Error, std::exchange(tl.errorStream, &stream));
}


void Log::_unstream(Stream & stream) noexcept
{
	switch (stream.type_) {
	case Stream::Type::Output:
		tl.outputStream = stream.previous_;
		break;
	case Stream::Type::Error:
		tl.errorStream = stream.previous_;
		break;
	}
}


void Log::_printContext(const Context & context, const uint32_t timestamp,
		std::ostream & s) noexcept
{
	{
		// print level
		static const std::string_view names[int(Level::Max)] = {
			"", "FAT", "ERR", "INF", "DBG"
		};
		s << names[int(context.level)] << " ";
	}

#ifdef LOG_PREFIX
	// print (prefix)
	s << "(" << LOG_PREFIX << ") ";
#endif

	// thread id
	{
		char tbuffer[10];
		std::snprintf(tbuffer, sizeof(tbuffer), "%4d", context.threadId);
		s << tbuffer << " ";
	}

	// print [timestamp]
	{
		char timeBuffer[20];
#define LOG_USE_TIMESTAMP_QUOTES
#ifdef LOG_USE_TIMESTAMP_QUOTES
		std::snprintf(timeBuffer, sizeof(timeBuffer),
				"[%03u'%03u'%03u] ",
				(timestamp / 1000000) % 1000,
				(timestamp / 1000) % 1000,
				timestamp % 1000);
#else
		std::snprintf(timeBuffer, sizeof(timeBuffer), "[%09u] ", timestamp);
#endif

		s << timeBuffer;
	}

	const int fileLineBeginPos = s.tellp();
	s << context.file << ":" << context.line;
	s << " " << context.func;
	const int fileLineLength = int(s.tellp()) - fileLineBeginPos;

	// align log content
	s << std::setfill(' ') << std::right <<
			std::setw(std::max(0, kFileAlignment - fileLineLength)) << " -";
}


void Log::print(const Context & context, const uint32_t timestamp,
		const std::string_view & text, const Stream::Type streamType) noexcept
{
	std::ostringstream s;

#ifdef LOG_COLOR
	_colorBegin(s);
#endif

	if (global_.printContext) {
		_printContext(context, timestamp, s);

		if (!text.empty()) {
			s << ' ';
		}
	}

#ifdef LOG_COLOR
	if (streamType == Stream::Type::Error) {
		// draw red
		_colorCustom(s, Color::Red);
	} else {
		// draw default
		_colorReset(s);
	}
#endif

	if (!text.empty()) {
		s.write(tl.indentBuffer.data(), tl.indentSize);
		s.write(text.data(), text.size());
	}

	s.write("\n", 1);

#ifdef LOG_COLOR
	if (streamType == Stream::Type::Error) {
		_colorReset(s);
	}
#endif

	if (streamType == Stream::Type::Error) {
		// force cerr and cout to flush first to simplify console output
		streamForType(Stream::Type::Output).flush();
		streamForType(Stream::Type::Error).flush();
	}

	const std::string str = s.str();
	std::ostream & ostream = streamForType(streamType);
	ostream.write(str.c_str(), str.size());
	ostream.flush();
}


#ifdef LOG_COLOR
void Log::_colorCustom(std::ostream & ostream, const Color color) noexcept
{
	if (!global_.enableColors) return;
	ostream << "\033[" << (30 + int(color)) << "m";
}


void Log::_colorBegin(std::ostream & ostream) noexcept
{
	const Color color = tl.color != Color::Null ? tl.color : Color::LOG_COLOR;
	_colorCustom(ostream, color);
}


void Log::_colorReset(std::ostream & ostream) noexcept
{
	if (!global_.enableColors) return;
	ostream << "\033[0m";
}
#endif


void Log::printMemorySize(const uint64_t v, const bool isNegative, const bool minusOne,
		const int nameOffset, const int min, const int max, std::ostream & ostream) noexcept
{
	static const char names[] = "BKMGTPE";

	if (v == 0) {
		ostream << "0";
		return;
	}

	if (isNegative) {
		ostream << '-';
		if (minusOne) {
			ostream << '(';
		}
	}
	for (int i = max; i >= min; --i) {
		if (i != max) {
			ostream << "'";
		}
		ostream << Log::memorySizeSegment(v, i);
		ostream << names[nameOffset + i];
		if (i != 0) {
			ostream << 'B';
		}
	}
	if (minusOne) {
		ostream << " - 1";
	}
	if (isNegative) {
		if (minusOne) {
			ostream << ')';
		}
	}
}


void Log::printLiteralNumber(const uint64_t v, const bool isNegative,
		std::ostream & ostream) noexcept
{
	if (v == 0) {
		ostream << "0";
		return;
	}

	if (isNegative) {
		ostream << '-';
	}

	uint16_t segments[7];
	int max = 0;
	for (uint64_t tv = v; tv != 0; tv = tv / 1000) {
		segments[max] = tv % 1000;
		max++;
	}
	max--;

	for (int i = max; i >= 0; i--) {
		if (i != max) {
			ostream << '\'';
			ostream << std::setw(3) << std::setfill('0');
		}
		ostream << segments[i];
	}
	ostream << std::setw(0);
}
