/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <cstdint>





inline static constexpr int kServerMaxBufferCount = 1000;

enum class ServerMessageType : int32_t {
	Info,
	GrantFrame,
	Quit,
};

struct ServerMessageInfo {
	int32_t bufferCount;
	int32_t maxPresentingCount;
	int32_t hasNv12;

	int32_t rgbaFrameWidth;
	int32_t rgbaFrameHeight;

	int32_t nv12FrameWidth;
	int32_t nv12FrameHeight;
};

struct ServerBufferInfo {
	uint32_t handle;
	int32_t pitch;
	int64_t dataSize;
};

struct ServerMessageGrantFrame {
	int32_t bufferIndex;
};




enum class ClientMessageType : int32_t {
	Quit,
	RequestFrame,
	ReleaseFrame,
};

struct ClientMessageRequestFrame {
};

struct ClientMessageReleaseFrame {
	int32_t bufferIndex;
};
