/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <utility>
#include <vector>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "GLDynamicFunctions.hpp"
#include "Log.hpp"




#ifndef EGL_NO_CONFIG_KHR
#define EGL_NO_CONFIG_KHR EGLConfig(0)
#endif




struct EglConfigAttributeName {
	EGLint key;
	std::string_view str;
};

static constexpr std::array kEglConfigAttributes = {
#define DEFINE_CONFIG_ATTRIBUTE(x) EglConfigAttributeName{ .key = x, .str = #x },
	DEFINE_CONFIG_ATTRIBUTE(EGL_RED_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_GREEN_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_BLUE_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_ALPHA_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_DEPTH_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_STENCIL_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_SURFACE_TYPE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_RENDERABLE_TYPE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_ALPHA_MASK_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_BIND_TO_TEXTURE_RGB)
	DEFINE_CONFIG_ATTRIBUTE(EGL_BIND_TO_TEXTURE_RGBA)
	DEFINE_CONFIG_ATTRIBUTE(EGL_BUFFER_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_COLOR_BUFFER_TYPE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_CONFIG_CAVEAT)
	DEFINE_CONFIG_ATTRIBUTE(EGL_CONFIG_ID)
	DEFINE_CONFIG_ATTRIBUTE(EGL_CONFORMANT)
	DEFINE_CONFIG_ATTRIBUTE(EGL_LEVEL)
	DEFINE_CONFIG_ATTRIBUTE(EGL_LUMINANCE_SIZE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_MAX_PBUFFER_WIDTH)
	DEFINE_CONFIG_ATTRIBUTE(EGL_MAX_PBUFFER_HEIGHT)
	DEFINE_CONFIG_ATTRIBUTE(EGL_MAX_PBUFFER_PIXELS)
	DEFINE_CONFIG_ATTRIBUTE(EGL_MAX_SWAP_INTERVAL)
	DEFINE_CONFIG_ATTRIBUTE(EGL_MIN_SWAP_INTERVAL)
	DEFINE_CONFIG_ATTRIBUTE(EGL_NATIVE_RENDERABLE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_NATIVE_VISUAL_ID)
	DEFINE_CONFIG_ATTRIBUTE(EGL_NATIVE_VISUAL_TYPE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_SAMPLE_BUFFERS)
	DEFINE_CONFIG_ATTRIBUTE(EGL_SAMPLES)
	DEFINE_CONFIG_ATTRIBUTE(EGL_TRANSPARENT_TYPE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_TRANSPARENT_RED_VALUE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_TRANSPARENT_GREEN_VALUE)
	DEFINE_CONFIG_ATTRIBUTE(EGL_TRANSPARENT_BLUE_VALUE)
#undef DEFINE_CONFIG_ATTRIBUTE
};


#if defined(USE_DYNAMIC_LOADER)
#elif defined(EGL_VERSION_1_5)
inline std::vector<EGLAttrib> eglAttribFromInt(const EGLint * const a)
{
	std::vector<EGLAttrib> al;
	for (const EGLint * i = a;; ++i) {
		al.push_back(EGLAttrib(*i));
		if (*i == EGL_NONE) break;
	}
	return al;
}

inline EGLImageKHR eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint * a) {
	if (a) {
		return eglCreateImage(dpy, ctx, target, buffer, eglAttribFromInt(a).data());
	} else {
		return eglCreateImage(dpy, ctx, target, buffer, nullptr);
	}
}
inline EGLBoolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image) { return eglDestroyImage(dpy, image); }
inline EGLSyncKHR eglCreateSyncKHR(EGLDisplay dpy, EGLenum type, const EGLint * a) {
	if (a) {
		return eglCreateSync(dpy, type, eglAttribFromInt(a).data());
	} else {
		return eglCreateSync(dpy, type, nullptr);
	}
}
inline EGLBoolean eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR sync) { return eglDestroySync(dpy, sync); }
inline EGLint EGLAPIENTRY eglClientWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout) { return eglClientWaitSync(dpy, sync, flags, timeout); }
#endif




struct EglInfo {
	EGLDisplay display;
//	EGLConfig config;
	EGLSurface surface;
	EGLContext context;
};




class EglDisplay {
public:
	static EglDisplay create(EGLenum platform, EGLNativeDisplayType nativeDisplay,
			const EGLint * attribs);

	EglDisplay(EglDisplay && other) noexcept;

	~EglDisplay() noexcept;

	EGLenum platform() const noexcept { return platform_; }

	EGLDisplay handle() const noexcept { return display_; }
	operator EGLDisplay() const noexcept { return display_; }

	int major() const noexcept { return major_; }
	int minor() const noexcept { return minor_; }

	operator bool() const noexcept { return display_ != EGL_NO_DISPLAY; }

	EglDisplay & operator=(const EglDisplay &) = delete;
	EglDisplay(const EglDisplay &) = delete;

private:
	struct Info;

	static Info _createInfo(EGLenum platform, EGLNativeDisplayType nativeDisplay,
			const EGLint * attribs);

	EglDisplay(EGLenum platform, const Info & info) noexcept;

	const EGLenum platform_;
	EGLDisplay display_;
	const int major_;
	const int minor_;
};




class EglSurface {
public:
	static EglSurface createSurfaceless(EGLDisplay display);
	static EglSurface createPbuffer(EGLDisplay display, const EGLConfig config,
			const EGLint * attribs);
	static EglSurface createWindow(EGLDisplay display, const EGLConfig config,
			EGLNativeWindowType nativeWindow, const EGLint * attribs);

	EglSurface(EglSurface && other) noexcept;

	~EglSurface() noexcept;

	EGLSurface handle() const noexcept { return surface_; }
	operator EGLSurface() const noexcept { return surface_; }

	EGLDisplay display() const noexcept { return display_; }

	operator bool() const noexcept { return surface_ != EGL_NO_SURFACE; }

	EglSurface & operator=(const EglSurface &) = delete;
	EglSurface(const EglSurface &) = delete;

private:
	EglSurface(EGLDisplay display, EGLSurface surface) noexcept;

	const EGLDisplay display_;
	EGLSurface surface_;
};




class EglContext {
public:
	EglContext(EGLDisplay display, EGLContext context) noexcept;
	~EglContext() noexcept;

	EGLContext handle() const noexcept { return context_; }
	operator EGLContext() const noexcept { return context_; }

	EGLDisplay display() const noexcept { return display_; }

	operator bool() const noexcept { return context_ != EGL_NO_CONTEXT; }

private:
	const EGLDisplay display_;
	const EGLContext context_;
};




#ifdef SDK_USE_DMA
class EglImage {
public:
	EglImage() noexcept = default;

	EglImage(EGLDisplay display, EGLImageKHR image) noexcept :
		display_(display),
		image_(image)
	{}

	EglImage(EglImage && other) noexcept :
		display_(std::exchange(other.display_, EGL_NO_DISPLAY)),
		image_(std::exchange(other.image_, EGL_NO_IMAGE_KHR))
	{}

	EglImage & operator=(EglImage && other) noexcept
	{
		std::swap(display_, other.display_);
		std::swap(image_, other.image_);
		return *this;
	}

	~EglImage() noexcept
	{
		if (image_ != EGL_NO_IMAGE_KHR) {
			GL_DYNAMIC_FUNCTIONS_NAMESPACE eglDestroyImageKHR(display_, image_);
		}
	}

	EGLImageKHR handle() const noexcept { return image_; }
	operator EGLImageKHR() const noexcept { return image_; }

	EGLDisplay display() const noexcept { return display_; }

	operator bool() const noexcept { return image_ != EGL_NO_IMAGE_KHR; }

	EglImage(const EglImage &) = delete;
	void operator=(const EglImage &) = delete;

private:
	EGLDisplay display_ = EGL_NO_DISPLAY;
	EGLImageKHR image_ = EGL_NO_IMAGE_KHR;
};
#endif




class EglSync {
public:
	static EglSync create(EGLDisplay display)
	{
		const EGLSyncKHR sync = GL_DYNAMIC_FUNCTIONS_NAMESPACE eglCreateSyncKHR(display, EGL_SYNC_FENCE_KHR, nullptr);
		CHECK(sync != EGL_NO_SYNC_KHR)
		return EglSync(display, sync);
	}

	EglSync() noexcept = default;

	EglSync(EglSync && other) noexcept :
		display_(std::exchange(other.display_, EGL_NO_DISPLAY)),
		sync_(std::exchange(other.sync_, EGL_NO_SYNC_KHR))
	{}

	EglSync & operator=(EglSync && other) noexcept
	{
		display_ = std::move(other.display_);
		std::swap(sync_, other.sync_);
		return *this;
	}

	~EglSync() noexcept
	{
		if (sync_ != EGL_NO_SYNC_KHR) {
			GL_DYNAMIC_FUNCTIONS_NAMESPACE eglDestroySyncKHR(display_, sync_);
		}
	}

	EGLSyncKHR handle() const noexcept { return sync_; }
	operator EGLSyncKHR() const noexcept { return sync_; }

	EGLDisplay display() const noexcept { return display_; }

	operator bool() const noexcept { return sync_ != EGL_NO_SYNC_KHR; }

	EglSync(const EglSync &) = delete;
	void operator=(const EglSync &) = delete;

private:
	EglSync(EGLDisplay display, EGLSyncKHR sync) noexcept :
		display_(display),
		sync_(sync)
	{}

	EGLDisplay display_ = EGL_NO_DISPLAY;
	EGLSyncKHR sync_ = EGL_NO_SYNC_KHR;
};




extern void dumpEglConfigs(EGLDisplay display, const std::vector<EGLConfig> & configs) noexcept;

extern void checkEGLError(const char * file, int line, const char * func);

#define CHECK_EGL_ERROR() \
	checkEGLError(__FILE__, __LINE__, __FUNCTION__);
