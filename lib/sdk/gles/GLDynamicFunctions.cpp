
#include "GLDynamicFunctions.hpp"

#include "Log.hpp"




namespace detail::GLDynamicFunctions {

using Pointer = void (*)();

template <typename F>
static F req(const char * const name)
{
	const Pointer p = (Pointer)eglGetProcAddress(name);
	CHECK(p, "Not found:", name)
	return reinterpret_cast<F>(p);
}

}


const GLDynamicFunctions & GLDynamicFunctions::get()
{
	using namespace detail::GLDynamicFunctions;

	#define REQ(name) .name = req<decltype(GLDynamicFunctions::name)>(#name),

	static const auto f = [] () -> GLDynamicFunctions {
		return GLDynamicFunctions {
			// EGL_KHR_image
			REQ(eglCreateImageKHR)
			REQ(eglDestroyImageKHR)

			// EGL_KHR_fence_sync
			REQ(eglCreateSyncKHR)
			REQ(eglDestroySyncKHR)
			REQ(eglClientWaitSyncKHR)

			// EGL_EXT_platform_base
			REQ(eglGetPlatformDisplayEXT)
			REQ(eglCreatePlatformWindowSurfaceEXT)
			REQ(eglCreatePlatformPixmapSurfaceEXT)

			// GL_OES_EGL_image
			REQ(glEGLImageTargetTexture2DOES)
		};
	}();

	return f;
}
