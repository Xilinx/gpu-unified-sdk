
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
