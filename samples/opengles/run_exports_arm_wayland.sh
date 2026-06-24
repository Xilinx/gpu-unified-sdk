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

# Mali user-space driver location (libEGL / libGLESv2 live here on the target).
export MALI_LIBS=/usr/lib
export MALI_INCLUDE=/usr/include
export LD_LIBRARY_PATH=/usr/lib/:${LD_LIBRARY_PATH}

# Select the Wayland EGL platform at runtime.
export EGL_PLATFORM=wayland

# A Wayland client finds the compositor socket at
# "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY". Prefer the per-user runtime dir; if it has
# no wayland socket, fall back to a shared one at /run/wayland-0.
if [ -z "${XDG_RUNTIME_DIR}" ]; then
    export XDG_RUNTIME_DIR="/run/user/$(id -u)"
fi
if [ ! -S "${XDG_RUNTIME_DIR}/wayland-0" ] && [ -S /run/wayland-0 ]; then
    export XDG_RUNTIME_DIR=/run
fi

export WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-wayland-0}
