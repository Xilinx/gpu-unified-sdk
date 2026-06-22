
# OpenGL ES samples
#
# Ported from ssw-mktg/GPU_SDK_OpenGLES. The directory layout mirrors the legacy
# SDK (samples grouped by GLES version), with folder/target names lowercased.
# Each sample has a self-contained CMakeLists.txt (explicit add_executable +
# link + asset staging), as in the legacy per-sample build files.
#
# All samples build on the vendored SdkGlesFramework (lib/sdk/GlesFramework):
# the legacy CoreFramework (EGL + Wayland windowing + GLSL compile + matrix /
# font / texture helpers).

# Window size, passed to samples as WINDOW_W / WINDOW_H compile definitions
# (default 1920x1080), exactly like the legacy build.
set(GLES_WINDOW_W "1920" CACHE STRING "OpenGL ES sample window width")
set(GLES_WINDOW_H "1080" CACHE STRING "OpenGL ES sample window height")

# Shared FontShaders folder (font.vert / font.frag / font.raw) used by the font
# engine and several samples. Samples stage this next to their binary.
set(GLES_FONT_SHADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../lib/sdk/GlesFramework/FontShaders")

add_subdirectory(opengl_es_2.0)
add_subdirectory(opengl_es_3.0)
