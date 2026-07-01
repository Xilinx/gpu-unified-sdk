/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "ArgumentParser.hpp"

#include "Log.hpp"
#include "Parser.hpp"




int ArgumentParser::Context::takeInt()
{
	const std::string_view currentArg = argv[argi];
	CHECK(i < argc - 1, "Expecting int after:", currentArg)
	i++;
	const auto intValue = std::string_view(argv[i]);
	return Parser::getInt(intValue);
}


float ArgumentParser::Context::takeFloat()
{
	const std::string_view currentArg = argv[argi];
	CHECK(i < argc - 1, "Expecting float after:", currentArg)
	i++;
	const auto floatValue = std::string_view(argv[i]);
	return Parser::getFloat(floatValue);
}


bool ArgumentParser::Context::takeBool()
{
	const std::string_view currentArg = argv[argi];
	CHECK(i < argc - 1, "Expecting bool after:", currentArg)
	i++;
	const auto boolValue = std::string_view(argv[i]);
	return Parser::getBool(boolValue);
}


std::string_view ArgumentParser::Context::takeStr()
{
	const std::string_view currentArg = argv[argi];
	CHECK(i < argc - 1, "Expecting string after:", currentArg)
	i++;
	return std::string_view(argv[i]);
}


void ArgumentParser::Context::parse(const Callback & cb)
{
	for (; i < argc; ++i) {
		argi = i;
		const auto arg = std::string_view(argv[i]);
		cb(*this, arg);
	}
}


void ArgumentParser::parse(int argc, char ** argv, const Callback & cb)
{
	try {
		Context {
			.argc = argc,
			.argv = argv,
			.i = 1,
		}.parse(cb);
	} catch (const Exception & e) {
		e.print();

		ERROR("Arguments:")
		for (int i = 0; i < argc; ++i) {
			ERROR("   ", argv[i])
		}

		throw;
	}
}
