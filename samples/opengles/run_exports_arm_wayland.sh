#! /bin/bash
#
# Runtime environment for the OpenGL ES samples on the aarch64 target using the
# Wayland windowing backend.
#
# Source this on the target (or in your run environment) before launching a
# sample that was built with -D SDK_Platform=wayland:
#
#     source run_exports_arm_wayland.sh
#     ./trianglesample
#
# Ported from ssw-mktg/GPU_SDK_OpenGLES/run_exports_arm_wayland.sh.

# Mali user-space driver location (libEGL / libGLESv2 live here on the target).
export MALI_LIBS=/usr/lib
export MALI_INCLUDE=/usr/include
export LD_LIBRARY_PATH=/usr/lib/:${LD_LIBRARY_PATH}

# Select the Wayland EGL platform at runtime.
export EGL_PLATFORM=wayland
export WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-wayland-0}
