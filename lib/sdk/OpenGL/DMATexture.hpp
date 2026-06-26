/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "DMAUtils.hpp"
#include "EGLUtils.hpp"
#include "GLUtils.hpp"




class DmaTexture {
public:
	enum class Target : GLenum {
		Null               = 0,
		Texture2D          = GL_TEXTURE_2D,
		TextureExternalOes = GL_TEXTURE_EXTERNAL_OES,
	};

	struct Params {
		const EGLint * extraImageAttributes = nullptr;
	};

	static DmaTexture null() noexcept { return DmaTexture(Target::Null, {}, TextureId::null()); }

	static DmaTexture create(EGLDisplay display, const Dma::Buffer & dmaBuffer, Target target,
			const Params & params);

	static DmaTexture adapt(EglImage && eglImage, Target target);

	~DmaTexture() noexcept;

	DmaTexture(DmaTexture && other) noexcept :
		target_(std::move(other.target_)),
		eglImage_(std::move(other.eglImage_)),
		textureId_(std::move(other.textureId_))
	{}

	DmaTexture & operator=(DmaTexture && other) noexcept
	{
		std::swap(target_, other.target_);
		std::swap(eglImage_, other.eglImage_);
		std::swap(textureId_, other.textureId_);
		return *this;
	}

	bool isNull() const noexcept { return textureId_.isNull(); }

	Target target() const noexcept { return target_; }
	EGLImageKHR eglImage() const noexcept { return eglImage_; }
	GLuint textureId() const noexcept { return textureId_; }

private:
	DmaTexture(Target target, EglImage && eglImage, TextureId && textureId) noexcept :
		target_(target),
		eglImage_(std::move(eglImage)),
		textureId_(std::move(textureId))
	{}

	Target target_;
	EglImage eglImage_;
	TextureId textureId_;
};




class DmaFbo {
public:
	static DmaFbo null() noexcept { return DmaFbo(DmaTexture::null(), FboId::null()); }

	static DmaFbo create(EGLDisplay, const Dma::Buffer & dmaBuffer);

	~DmaFbo() noexcept;

	DmaFbo(DmaFbo && other) noexcept :
		dmaTexture_(std::move(other.dmaTexture_)),
		fboId_(std::move(other.fboId_))
	{}

	DmaFbo & operator=(DmaFbo && other) noexcept
	{
		std::swap(dmaTexture_, other.dmaTexture_);
		std::swap(fboId_, other.fboId_);
		return *this;
	}

	const DmaTexture & dmaTexture() const noexcept { return dmaTexture_; }
	GLuint fboId() const noexcept { return fboId_; }

private:
	DmaFbo(DmaTexture && dmaTexture, FboId && fboId) noexcept :
		dmaTexture_(std::move(dmaTexture)),
		fboId_(std::move(fboId))
	{}

	DmaTexture dmaTexture_;
	FboId fboId_;
};
