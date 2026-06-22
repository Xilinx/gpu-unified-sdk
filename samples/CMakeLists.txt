
# Make installed sample binaries find the SDK shared libraries (installed into
# "lib", see lib/sdk/CMakeLists.txt) via an $ORIGIN-relative RPATH, so they run
# without LD_LIBRARY_PATH. Two entries cover both supported install layouts:
#   bin/<sample>/<binary>      + lib/   -> $ORIGIN/../../lib
#   <api>/<sample>/<binary>    + <api>/lib/ (packaged layout) -> $ORIGIN/../lib
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN/../../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

add_subdirectory(opencl)
add_subdirectory(vulkan)
add_subdirectory(opengles)
