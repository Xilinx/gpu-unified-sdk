
#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>




struct GLDynamicFunctions {
	// EGL_KHR_image
	PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = nullptr;
	PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = nullptr;

	// EGL_KHR_fence_sync
	PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR = nullptr;
	PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR = nullptr;
	PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR = nullptr;

	// EGL_EXT_platform_base
	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = nullptr;
	PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC eglCreatePlatformWindowSurfaceEXT = nullptr;
	PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC eglCreatePlatformPixmapSurfaceEXT = nullptr;

	// GL_OES_EGL_image
	PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = nullptr;

	static const GLDynamicFunctions & get();
};




#define GL_DYNAMIC_FUNCTIONS_NAMESPACE GLDynamicFunctions::get().
