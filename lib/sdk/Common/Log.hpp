/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <any>
#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#ifdef LIBDEMO_USE_STACKTRACE
#include <stacktrace>
#endif
#include <stdexcept>
#include <string_view>
#include <utility>




class Log {
public:
	enum class Color {
		Null    = -1,
		Black   = 0,
		Red     = 1,
		Green   = 2,
		Yellow  = 3,
		Blue    = 4,
		Magenta = 5,
		Cyan    = 6,
		White   = 7,
	};

	enum class Level {
		None,
		Fatal,
		Error,
		Info,
		Debug,
		Max, // must be last
	};

	inline static const std::chrono::steady_clock::time_point kAppStartTime =
			std::chrono::steady_clock::now();

	static int getCurrentThreadId() noexcept;

	struct Init {
		std::optional<bool> enableColors = {};
		std::optional<Level> level = {};
		std::optional<bool> printContext = {};
	};

	struct Stacktrace {
#ifdef LIBDEMO_USE_STACKTRACE
		std::stacktrace st;
#endif
	};

	struct Context {
		Level level;
		std::string_view file;
		int line;
		std::string_view func;
		int threadId;
		Stacktrace stacktrace;
	};

	struct NoSpaceOnce{};

	class Indent {
	public:
		Indent() noexcept : size_(0) {}
		Indent(Indent && other) noexcept : size_(std::exchange(other.size_, 0)) {}
		Indent & operator=(Indent && other) noexcept { std::swap(size_, other.size_); return *this; }
		~Indent() noexcept { Log::_unindent(size_); }

		Indent(const Indent &) = delete;
		void operator=(const Indent &) = delete;

	private:
		Indent(int size) noexcept : size_(size) {}

		int size_;

		friend class Log;
	};

	class Stream {
	public:
		enum class Type { Output, Error };
		inline static constexpr Type kInvalidType = Type(-1);

		Stream() noexcept : type_(kInvalidType), previous_(nullptr) {}
		Stream(Stream && other) noexcept :
			type_(std::exchange(other.type_, kInvalidType)),
			previous_(std::exchange(other.previous_, nullptr))
		{}
		Stream & operator=(Stream && other) noexcept {
			std::swap(type_, other.type_),
			std::swap(previous_, other.previous_);
			return *this;
		}
		~Stream() noexcept { Log::_unstream(*this); }

		Stream(const Stream &) = delete;
		void operator=(const Stream &) = delete;

	private:
		Stream(Type type, std::ostream * previous) noexcept :
			type_(type), previous_(previous) {}

		Type type_;
		std::ostream * previous_;

		friend class Log;
	};

	inline static constexpr std::array kDirPrefixes = {
#ifdef LOG_ROOT_DIR
		std::string_view(LOG_ROOT_DIR "/"),
#endif
#ifdef LOG_EXTRA_1_DIR
		std::string_view(LOG_EXTRA_1_DIR "/"),
#endif
	};

	static Context context(Level level, std::string_view file, int line, std::string_view func,
			Stacktrace && stacktrace) noexcept;

	static void setThreadLocalColor(Color color) noexcept;

	struct detail {
		template <typename T> struct Hex { const T & value; int size; };
		template <typename T> struct Bits { const T & value; int size; };
		template <typename T> struct Wrap { const T & value; const char * b; const char * e; };
		template <typename T> struct MemorySize { const T & value; };
		template <typename T> struct LiteralNumber { const T & value; };
		struct FourCC { uint32_t value; };
		struct Errno { int err; };
		struct Format { std::array<char, 1024> buffer; int size; };
		struct Attach { std::any any; };
		struct Binary { const uint8_t * data; int size; };
		struct Bool { bool value; };
	};

	static Indent indent(const std::string_view & value = global_.indent) noexcept;

	static std::string_view completeIndent() noexcept;

	static Stream output(std::ostream & stream) noexcept;
	static Stream error(std::ostream & stream) noexcept;

	template <typename T>
	static detail::Hex<T> hex(const T & value) noexcept {
		return detail::Hex<T>{ .value = value, .size = sizeof(T) };
	}

	template <typename T>
	static detail::Hex<T> hex(const T & value, int size) noexcept {
		return detail::Hex<T>{ .value = value, .size = size };
	}

	template <typename T>
	static detail::Bits<T> bits(const T & value) noexcept {
		return detail::Bits<T>{ .value = value, .size = sizeof(T) * 8 };
	}

	template <typename T>
	static detail::Bits<T> bits(const T & value, int size) noexcept {
		return detail::Bits<T>{ .value = value, .size = size };
	}

	template <typename T>
	static detail::Binary binary(const T & value) noexcept {
		return detail::Binary{
			.data = reinterpret_cast<const uint8_t*>(&value), .size = sizeof(T)
		};
	}

	static detail::Binary binary(const void * data, int size) noexcept {
		return detail::Binary{
			.data = reinterpret_cast<const uint8_t*>(data), .size = size
		};
	}

	template <typename T>
	static detail::Bool boolean(const T & value) noexcept {
		return detail::Bool{ .value = !!value };
	}

	template <typename T>
	static detail::Wrap<T> quoted(const T & value) noexcept {
		return detail::Wrap<T>{ .value = value, .b = "\"", .e = "\"" };
	}

	template <typename T>
	static detail::Wrap<T> braced(const T & value) noexcept {
		return detail::Wrap<T>{ .value = value, .b = "(", .e = ")" };
	}

	template <typename T>
	static detail::Wrap<T> squared(const T & value) noexcept {
		return detail::Wrap<T>{ .value = value, .b = "[", .e = "]" };
	}

	template <typename T>
	static detail::MemorySize<T> memorySize(const T & value) noexcept {
		return detail::MemorySize<T>{ .value = value };
	}

	template <typename T>
	static detail::LiteralNumber<T> literalNumber(const T & value) noexcept {
		return detail::LiteralNumber<T>{ .value = value };
	}

	static detail::FourCC fourcc(uint32_t value) noexcept {
		return detail::FourCC{ .value = value };
	}

	static detail::Errno err() noexcept {
		return detail::Errno{ .err = errno };
	}

	static detail::Errno err(int err) noexcept {
		return detail::Errno{ .err = err };
	}

	template <typename... T>
	static detail::Format format(const char * format, const T &... values) noexcept {
		detail::Format f;
		f.size = std::snprintf(f.buffer.data(), f.buffer.size(), format, values...);
		return f;
	}

	template <typename T>
	static detail::Attach attach(T && value) noexcept {
		return detail::Attach{ .any = std::move(value) };
	}

	static void init(const Init & init) noexcept;

	static bool levelEnabled(Level l) noexcept { return int(l) <= int(global_.level); }

	Log(const Context & context) noexcept :
		context_(context),
		timestamp_(std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now() - kAppStartTime).count() % 1000000000)
	{
	}

	const Context & context() const noexcept { return context_; }
	uint32_t timestamp() const noexcept { return timestamp_; }
	std::any & attach() noexcept { return attach_; }

	std::string str() noexcept
	{
		return stream_.str();
	}

	void print(Stream::Type streamType) noexcept
	{
		print(context_, timestamp_, str(), streamType);
	}

	template <typename T>
	void append(const T & value) noexcept;

	template <typename T>
	Log & operator,(const T & value) noexcept
	{
		append(value);
		return *this;
	}

	static void print(const Context & context, uint32_t timestamp,
			const std::string_view & text, Stream::Type streamType) noexcept;

	static void printErrno(int err, std::ostream & stream) noexcept;

	static void printMemorySize(uint64_t v, bool isNegative, bool minusOne,
			int nameOffset, int min, int max, std::ostream & ostream) noexcept;

	static void printLiteralNumber(uint64_t v, bool isNegative, std::ostream & ostream) noexcept;

	template <typename T>
	static T memorySizeSegment(const T & value, int i) noexcept {
		return (value >> (i * 10)) % 1024;
	}

	template <typename T>
	static uint64_t toPositiveUnsigned(const T & value) noexcept {
		using U = std::make_unsigned_t<T>;
		return value < 0 ?
			value == std::numeric_limits<T>::min() ?
				U(value) : U(-value) :
			U(value);
	}

private:
	using __CU8Ptr = const uint8_t *;
	using __MU8Ptr = uint8_t *;
	using __CI8Ptr = const char *;
	using __MI8Ptr = char *;

	template <typename T>
	struct Write {
		static void write(std::ostringstream & s, const T & value) noexcept
		{ s << value; }
	};

	struct Global {
		bool enableColors = false;
		Level level = Level::Debug;
		bool printContext = true;
		std::string_view indent = "    ";
	};

	static Global global_;

#ifdef LOG_COLOR
	static void _colorCustom(std::ostream & ostream, Color color) noexcept;
	static void _colorBegin(std::ostream & ostream) noexcept;
	static void _colorReset(std::ostream & ostream) noexcept;
#endif

	static void _unindent(int size) noexcept;
	static void _unstream(Stream & stream) noexcept;

	static void _printContext(const Context & context, uint32_t timestamp,
			std::ostream & ostream) noexcept;

	const Context context_;
	const uint32_t timestamp_;
	std::ostringstream stream_;
	bool needSpace_ = false;
	std::any attach_;
};




class NoLog {
public:
	template <typename T>
	NoLog & operator,(const T & value) noexcept
	{
		(void)value;
		return *this;
	}
};




inline Log::Context Log::context(const Level level, const std::string_view file, const int line,
		const std::string_view func, Stacktrace && stacktrace) noexcept
{
	std::string_view::size_type prefixSize = 0;
	for (const std::string_view & prefix : kDirPrefixes) {
		if (file.starts_with(prefix)) {
			prefixSize = prefix.size();
			break;
		}
	}
	return Context{
		.level = level,
		.file = file.substr(prefixSize),
		.line = line,
		.func = func,
		.threadId = Log::getCurrentThreadId(),
		.stacktrace = std::move(stacktrace),
	};
}


template <typename T>
inline std::ostream & operator <<(std::ostream & o, const Log::detail::Hex<T> & hex)
{
	using U = std::conditional_t<sizeof(T) == 1, uint16_t, std::make_unsigned_t<T>>;
	const auto flags = o.flags();
	o << "0x" << std::right << std::setfill('0') << std::setw(hex.size * 2)
			<< std::hex << U(hex.value);
	o.flags(flags);
	return o;
}


template <typename T>
inline std::ostream & operator <<(std::ostream & o, const Log::detail::Bits<T> & bits)
{
	using U = std::conditional_t<sizeof(T) == 1, uint16_t, std::make_unsigned_t<T>>;
	o << "0b";
	for (int i = bits.size - 1; i >= 0; --i) {
		o << ((U(bits.value) & (1 << i)) == 0 ? '0' : '1');
	}
	return o;
}


inline std::ostream & operator <<(std::ostream & o, const Log::detail::Binary & binary)
{
	struct S {
		inline static char hex(uint8_t i) noexcept {
			if (i < 10) {
				return '0' + i;
			} else {
				return 'a' + i - 10;
			}
		}
	};

	for (int i = 0; i < binary.size; ++i) {
		const uint8_t c = binary.data[i];
		o << S::hex(c >> 4) << S::hex(c & 0xf);
	}
	return o;
}


inline std::ostream & operator <<(std::ostream & o, const Log::detail::Bool & boolean)
{
	o << (boolean.value ? "true" : "false");
	return o;
}


template <typename T>
inline std::ostream & operator <<(std::ostream & o, const Log::detail::Wrap<T> & wrap)
{
	return o << wrap.b << wrap.value << wrap.e;
}


inline std::ostream & operator <<(std::ostream & o, const Log::detail::FourCC & fourcc)
{
	o.write(reinterpret_cast<const char*>(&fourcc.value), 4);
	return o;
}


inline std::ostream & operator <<(std::ostream & o, const Log::detail::Errno & e)
{
	Log::printErrno(e.err, o);
	return o;
}


inline std::ostream & operator <<(std::ostream & o, const Log::detail::Format & format)
{
	o.write(format.buffer.data(), format.size);
	return o;
}


template <typename T>
inline std::ostream & operator <<(std::ostream & o, const Log::detail::MemorySize<T> & m)
{
	static constexpr int kMaxSegment = (8 * sizeof(T)) / 10;

	const bool isNegative = m.value < 0;

	uint64_t v = Log::toPositiveUnsigned(m.value);

	const bool minusOne = Log::memorySizeSegment(m.value + 1, 0) == 0;

	int nameOffset = 0;

	if (minusOne) {
		if (m.value == std::numeric_limits<T>::max()) {
			v = (m.value >> 10) + 1;
			nameOffset += 1;
		} else {
			v += 1;
		}
	}

	int min = 0;
	int max = 0;

	if (v != 0) {
		for (min = 0; min <= kMaxSegment; ++min) {
			if (Log::memorySizeSegment(v, min) != 0) break;
		}

		for (max = kMaxSegment; max != min; --max) {
			if (Log::memorySizeSegment(v, max) != 0) break;
		}
	}

	Log::printMemorySize(v, isNegative, minusOne, nameOffset, min, max, o);

	return o;
}


template <typename T>
inline std::ostream & operator <<(std::ostream & o, const Log::detail::LiteralNumber<T> & m)
{
	const bool isNegative = m.value < 0;
	uint64_t v = Log::toPositiveUnsigned(m.value);
	Log::printLiteralNumber(v, isNegative, o);
	return o;
}


template <>
struct Log::Write<uint8_t> {
	static void write(std::ostream & s, const uint8_t & value) noexcept
	{
		s << int(value);
	}
};


template <>
struct Log::Write<Log::__CU8Ptr> {
	static void write(std::ostream & s, const __CU8Ptr & value) noexcept
	{
		s << (void*)(value);
	}
};


template <>
struct Log::Write<Log::__MU8Ptr> {
	static void write(std::ostream & s, const __MU8Ptr & value) noexcept
	{
		s << (void*)(value);
	}
};


template <>
struct Log::Write<Log::__CI8Ptr> {
	static void write(std::ostream & s, const __CI8Ptr & value) noexcept
	{
		if (!value) {
			s << "<null>";
		} else {
			s << value;
		}
	}
};


template <>
struct Log::Write<Log::__MI8Ptr> {
	static void write(std::ostream & s, const __MI8Ptr & value) noexcept
	{
		if (!value) {
			s << "<null>";
		} else {
			s << value;
		}
	}
};


template <int S>
struct Log::Write<uint8_t[S]> {
	static void write(std::ostringstream & s, const uint8_t (& value)[S]) noexcept
	{
		s << (void*)value;
	}
};


template <>
inline void Log::append<Log::detail::Attach>(const detail::Attach & value) noexcept
{
	attach_ = std::move(value.any);
}


template <>
inline void Log::append<Log::NoSpaceOnce>(const NoSpaceOnce &) noexcept
{
	needSpace_ = false;
}


template <typename T>
inline void Log::append(const T & value) noexcept
{
	if (needSpace_) {
		stream_ << " ";
	} else {
		needSpace_ = true;
	}
	Write<T>::write(stream_, value);
}



class Exception : public std::runtime_error {
public:
	Exception(const Log::Context & context, uint32_t timestamp, std::string && text,
			std::any && attach) noexcept :
		std::runtime_error(text),
		context_(context),
		timestamp_(timestamp),
		text_(std::move(text)),
		attach_(std::move(attach))
	{
	}

	const Log::Context & context() const noexcept { return context_; }
	uint32_t timestamp() const noexcept { return timestamp_; }
	std::string_view text() const noexcept { return text_; }
	const std::any & attach() const noexcept { return attach_; }

	void print() const noexcept
	{
		Log::print(context_, timestamp_, text_, Log::Stream::Type::Error);
	}

	void print(Log::Stream::Type streamType) const noexcept
	{
		Log::print(context_, timestamp_, text_, streamType);
	}

private:
	const Log::Context context_;
	const uint32_t timestamp_;
	const std::string text_;
	const std::any attach_;
};




class ExceptionMaker {
public:
	ExceptionMaker(const Log::Context & context) noexcept :
		log_(context)
	{
	}

	Log & log() noexcept { return log_; }

	Exception make() noexcept
	{
		return Exception(log_.context(), log_.timestamp(), log_.str(),
				std::move(log_.attach()));
	}

	template <typename T>
	ExceptionMaker & operator,(const T & value) noexcept
	{
		log_.append(value);
		return *this;
	}

private:
	Log log_;
};




#define LOG_ENABLED

#ifdef LOG_ENABLED

#ifdef LIBDEMO_USE_STACKTRACE
#	define LOG_MAKE_STACKTRACE Log::Stacktrace{ .st = std::stacktrace::current() }
#else
#	define LOG_MAKE_STACKTRACE Log::Stacktrace{}
#endif

#define FATAL(...) { \
		(Log(Log::context(Log::Level::Fatal, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), "Fatal:", \
				##__VA_ARGS__).print(Log::Stream::Type::Error); \
		abort(); \
	}

#define ASSERT(x, ...) { \
	if (!(x)) [[unlikely]] { \
		(Log(Log::context(Log::Level::Fatal, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), "Condition failed:", Log::quoted(#x), \
				##__VA_ARGS__).print(Log::Stream::Type::Error); \
		abort(); \
	}}

#define ERROR(...) { \
	if (Log::levelEnabled(Log::Level::Error)) { \
		(Log(Log::context(Log::Level::Error, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), \
				##__VA_ARGS__).print(Log::Stream::Type::Error); \
	}}

#define DEBUG(...) { \
	if (Log::levelEnabled(Log::Level::Debug)) { \
		(Log(Log::context(Log::Level::Debug, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), \
				##__VA_ARGS__).print(Log::Stream::Type::Output); \
	}}

#define INFO(...) { \
	if (Log::levelEnabled(Log::Level::Info)) { \
		(Log(Log::context(Log::Level::Info, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), \
				##__VA_ARGS__).print(Log::Stream::Type::Output); \
	}}

#else // LOG_ENABLED

#define FATAL(...) { (NoLog(), ##__VA_ARGS__); abort(); }
#define ASSERT(x, ...) { (NoLog(), ##__VA_ARGS__); if (!(x)) [[unlikely]] abort(); }
#define ERROR(...) (NoLog(), ##__VA_ARGS__);
#define DEBUG(...) (NoLog(), ##__VA_ARGS__);
#define INFO(...) (NoLog(), ##__VA_ARGS__);

#endif // LOG_ENABLED

#define CHECK(x, ...) \
	if (!(x)) [[unlikely]] { \
		throw (ExceptionMaker(Log::context(Log::Level::Error, __FILE__, __LINE__, __FUNCTION__, \
				LOG_MAKE_STACKTRACE)), \
				"Condition failed:", Log::quoted(#x), \
				##__VA_ARGS__).make(); \
	}

#define THROW(...) \
	throw (ExceptionMaker(Log::context(Log::Level::Error, __FILE__, __LINE__, __FUNCTION__, \
			LOG_MAKE_STACKTRACE)), \
			##__VA_ARGS__).make();
