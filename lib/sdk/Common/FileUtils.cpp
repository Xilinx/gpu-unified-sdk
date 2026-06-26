/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "FileUtils.hpp"

#include <fstream>

#include "Log.hpp"




File openFile(const std::filesystem::path & path, const char * mode)
{
	FILE * const fp = ::fopen(path.c_str(), mode);
	CHECK(fp, "Failed to open file:", path, "mode:", mode)
	return File(fp);
}


std::string readTextFile(const std::filesystem::path & path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	CHECK(file.good(), path)
	const auto size = file.tellg();
	file.seekg(0);
	std::string str;
	str.resize(size);
	file.read(&str[0], size);
	return str;
}


std::vector<uint8_t> readBinaryFile(const std::filesystem::path & path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	CHECK(file.good(), path)
	const auto size = file.tellg();
	file.seekg(0);
	std::vector<uint8_t> data;
	data.resize(size);
	file.read(reinterpret_cast<char*>(data.data()), size);
	return data;
}
