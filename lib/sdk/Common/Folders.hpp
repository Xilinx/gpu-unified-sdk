/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <filesystem>




struct Folders {
	static const Folders & get() noexcept;

	std::filesystem::path shareDir;

	// input
	std::filesystem::path shadersDir;
	std::filesystem::path imagesDir;
	std::filesystem::path modelsDir;

	// cache
	std::filesystem::path cacheDir;

	// output
	std::filesystem::path outputDir;

	// user
	std::filesystem::path userDir;
};
