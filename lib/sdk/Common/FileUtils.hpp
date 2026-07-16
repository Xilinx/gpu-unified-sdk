/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>




struct FileDeleter {
	void operator()(FILE * file) noexcept { ::fclose(file); }
};

using File = std::unique_ptr<FILE, FileDeleter>;

extern File openFile(const std::filesystem::path & path, const char * mode);

extern std::string readTextFile(const std::filesystem::path & path);
extern std::vector<uint8_t> readBinaryFile(const std::filesystem::path & path);
