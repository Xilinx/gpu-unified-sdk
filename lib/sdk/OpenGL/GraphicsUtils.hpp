
#pragma once

#include <filesystem>

#include <EGL/egl.h>

#include <Common/Geometry.hpp>

#ifdef SDK_USE_DMA
#include "DMATexture.hpp"
#include "DRMUtils.hpp"
#endif
#include "GLUtils.hpp"




struct RegularTexture {
public:
	static RegularTexture fromPng(const std::filesystem::path & path);

	TextureId textureId = TextureId::null();
	Size size = Size::zero();
};




struct CubemapTexture {
public:
	static CubemapTexture fromPng(const std::filesystem::path & prefixPath);

	TextureId textureId = TextureId::null();
	Size size = Size::zero();
};




#ifdef SDK_USE_DMA
struct DrmTexture {
	static DrmTexture fromPng(int drmDevFd, EGLDisplay eglDisplay,
			const std::filesystem::path & path);

	Drm::Buffer drmBuffer = Drm::Buffer::null();
	DmaTexture dmaTexture = DmaTexture::null();
};
#endif
