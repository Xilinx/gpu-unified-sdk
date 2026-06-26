/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "Parser.hpp"

#include <charconv>

#include "Log.hpp"




namespace detail {
struct ParserImpl {


static std::error_code checkResult(const std::from_chars_result & r,
		const std::string_view & s) noexcept
{
	std::error_code err = std::make_error_code(r.ec);
	if (err) {
		if (r.ptr != s.data() + s.size()) {
			err = std::make_error_code(std::errc::invalid_argument);
		}
	}
	return err;
}


template <typename T>
static T parseNumberErr(const std::string_view & s, std::error_code & err) noexcept
{
	T value;
	const std::from_chars_result r = std::from_chars(s.data(), s.data() + s.size(), value);
	err = checkResult(r, s);
	return value;
}


static bool parseBoolErr(const std::string_view & s, std::error_code & err) noexcept
{
	err = {};
	if (s == "1" || s == "true") return true;
	if (s == "0" || s == "false") return false;
	err = std::make_error_code(std::errc::invalid_argument);
	return false;
}


}; // ParserImpl
} // detail




int Parser::getInt(const std::string_view & s)
{
	std::error_code err;
	const int value = detail::ParserImpl::parseNumberErr<int>(s, err);
	CHECK(!err, "Invalid int:", Log::quoted(s))
	return value;
}


int Parser::getInt(const std::string_view & s, std::error_code & err) noexcept
{
	return detail::ParserImpl::parseNumberErr<int>(s, err);
}


float Parser::getFloat(const std::string_view & s)
{
	std::error_code err;
	const float value = detail::ParserImpl::parseNumberErr<float>(s, err);
	CHECK(!err, "Invalid float:", Log::quoted(s))
	return value;
}


float Parser::getFloat(const std::string_view & s, std::error_code & err) noexcept
{
	return detail::ParserImpl::parseNumberErr<float>(s, err);
}


bool Parser::getBool(const std::string_view & s)
{
	std::error_code err;
	const bool value = detail::ParserImpl::parseBoolErr(s, err);
	CHECK(!err, "Invalid bool:", Log::quoted(s))
	return value;
}


bool Parser::getBool(const std::string_view & s, std::error_code & err) noexcept
{
	return detail::ParserImpl::parseBoolErr(s, err);
}
