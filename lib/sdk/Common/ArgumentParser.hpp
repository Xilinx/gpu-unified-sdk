/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <functional>
#include <string_view>




class ArgumentParser {
public:
	struct Context;

	using Callback = std::function<void(Context & c, const std::string_view & arg)>;

	struct Context {
		const int argc;
		char ** const argv;
		int argi;
		int i;

		int takeInt();
		float takeFloat();
		bool takeBool();
		std::string_view takeStr();

		void parse(const Callback & cb);
	};

	static void parse(int argc, char ** argv, const Callback & cb);
};
