
find_package(DRM REQUIRED)

add_library(SdkOpenGL SHARED
	DMATexture.cpp
	DMAUtils.cpp
	DRMUtils.cpp
	EglSyncWaiter.cpp
	EGLUtils.cpp
	GLDynamicFunctions.cpp
	GLUtils.cpp
	GraphicsUtils.cpp
)

target_link_libraries(SdkOpenGL
	PUBLIC
		SdkIf

	PRIVATE
		drm
)
