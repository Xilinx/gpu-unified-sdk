/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "EGLUtils.hpp"

#include "GLDynamicFunctions.hpp"
#include "Log.hpp"




struct EglDisplay::Info {
	EGLDisplay display;
	int major, minor;
};


EglDisplay EglDisplay::create(const EGLenum platform, const EGLNativeDisplayType nativeDisplay,
		const EGLint * const attribs)
{
	const Info info = _createInfo(platform, nativeDisplay, attribs);
	return EglDisplay(platform, info);
}


EglDisplay::Info EglDisplay::_createInfo(const EGLenum platform,
		const EGLNativeDisplayType nativeDisplay, const EGLint * const attribs)
{
	const EGLDisplay display = GL_DYNAMIC_FUNCTIONS_NAMESPACE
			eglGetPlatformDisplayEXT(platform, nativeDisplay, attribs);
	CHECK(display != EGL_NO_DISPLAY, "eglGetPlatformDisplayEXT");

	int major, minor;
	try {
		CHECK(eglInitialize(display, &major, &minor));
	} catch (const Exception &) {
		eglTerminate(display);
		throw;
	}

	return Info{
		.display = display,
		.major = major,
		.minor = minor,
	};
}


EglDisplay::EglDisplay(EglDisplay && other) noexcept :
	platform_(other.platform_),
	display_(std::exchange(other.display_, EGL_NO_DISPLAY)),
	major_(other.major_),
	minor_(other.minor_)
{
}


EglDisplay::EglDisplay(const EGLenum platform, const Info & info) noexcept :
	platform_(platform),
	display_(info.display),
	major_(info.major),
	minor_(info.minor)
{
}


EglDisplay::~EglDisplay() noexcept
{
	if (display_ != EGL_NO_DISPLAY) {
		eglTerminate(display_);
	}
}




EglSurface EglSurface::createSurfaceless(const EGLDisplay display)
{
	return EglSurface(display, EGL_NO_SURFACE);
}


EglSurface EglSurface::createPbuffer(const EGLDisplay display,
		const EGLConfig config, const EGLint * const attribs)
{
	const EGLSurface surface = eglCreatePbufferSurface(display, config, attribs);
	CHECK_EGL_ERROR()
	CHECK(surface != EGL_NO_SURFACE)
	return EglSurface(display, surface);
}


EglSurface EglSurface::createWindow(const EGLDisplay display, const EGLConfig config,
		const EGLNativeWindowType nativeWindow, const EGLint * const attribs)
{
	const EGLSurface surface = eglCreateWindowSurface(display, config,
			nativeWindow, attribs);
	CHECK_EGL_ERROR()
	CHECK(surface != EGL_NO_SURFACE)
	return EglSurface(display, surface);
}


EglSurface::EglSurface(EglSurface && other) noexcept :
	display_(other.display_),
	surface_(std::exchange(other.surface_, EGL_NO_SURFACE))
{
}


EglSurface::EglSurface(const EGLDisplay display, const EGLSurface surface) noexcept :
	display_(display),
	surface_(surface)
{
}


EglSurface::~EglSurface() noexcept
{
	if (surface_ != EGL_NO_SURFACE) {
		eglDestroySurface(display_, surface_);
	}
}




EglContext::EglContext(const EGLDisplay display, const EGLContext context) noexcept :
	display_(display),
	context_(context)
{
}


EglContext::~EglContext() noexcept
{
	eglDestroyContext(display_, context_);
}




static EGLint getEglConfigAttribute(const EGLDisplay display, const EGLConfig config,
		const EGLint attribute)
{
	EGLint value;
	const bool ok = eglGetConfigAttrib(display, config, attribute, &value);
	CHECK_EGL_ERROR()
	CHECK(ok, attribute)
	return value;
}


void dumpEglConfigs(const EGLDisplay display, const std::vector<EGLConfig> & configs) noexcept
{
	// memorize same values among all configs, for simpler comparison
	struct SameValue { bool checked = false; int value; bool same = true; };
	std::array<SameValue, kEglConfigAttributes.size()> sameValues;

	for (int i = 0; i < int(configs.size()); ++i) {
		const EGLConfig config = configs[i];
		const auto get = [config, display] (const EGLint a) {
			return getEglConfigAttribute(display, config, a);
		};
		for (int ei = 0; ei < kEglConfigAttributes.size(); ++ei) {
			const EglConfigAttributeName & a = kEglConfigAttributes[ei];
			SameValue & sameValue = sameValues[ei];
			const int value = get(a.key);
			if (!sameValue.checked) {
				sameValue.value = value;
				sameValue.checked = true;
			} else {
				if (sameValue.same) {
					sameValue.same = sameValue.value == value;
				}
			}
		}
	}

	{
		DEBUG("Shared EGL config attributes:")
		auto indent = Log::indent();
		for (int ei = 0; ei < kEglConfigAttributes.size(); ++ei) {
			const EglConfigAttributeName & a = kEglConfigAttributes[ei];
			const SameValue & sameValue = sameValues[ei];
			if (!sameValue.same) continue;
			DEBUG(a.str, ":", sameValue.value)
		}
	}

	for (int i = 0; i < int(configs.size()); ++i) {
		const EGLConfig & config = configs[i];
		const auto get = [config, display] (const EGLint a) {
			return getEglConfigAttribute(display, config, a);
		};
		DEBUG("EGL config:", Log::braced(i), config)
		auto indent = Log::indent();
		for (int ei = 0; ei < kEglConfigAttributes.size(); ++ei) {
			const EglConfigAttributeName & a = kEglConfigAttributes[ei];
			const SameValue & sameValue = sameValues[ei];
			if (sameValue.same) continue;
			DEBUG(a.str, ":", get(a.key))
		}
	}
}




void checkEGLError(const char * const file, const int line, const char * const func)
{
	const EGLint err = eglGetError();
	if (err != EGL_SUCCESS) [[unlikely]] {
		throw (ExceptionMaker(Log::context(Log::Level::Error, file, line, func,
				LOG_MAKE_STACKTRACE)),
				"EGL error:", Log::hex(err, 2)).make();
	}
}
