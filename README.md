
# Project structure

- cmake
  - modules - Additional CMake module wrappers
    - ecm   - Extra CMake Modules (used in Wayland protocol compilation)
- lib
  - sdk
    - common - Common graphical API agnostic helper classes
    - egl    - EGL helper classes
    - gles   - GLES helper classes
    - vulkan - Vulkan helper classes (RAII API)
    - vulkan-strings    - Vulkan API identifiers to string convertor (debugging purposes)
    - vulkan-vendor-ids - GPU vendor name resolution from PCI ID global database (debugging purposes)
- samples
  - gles
    - triangle  - Trivial triangle rendering
    - eglimage  - EGLImage rendering
    - vcu2      - Video decoding and rendering via shared dmabuf texture
    - occlusion - GLES 3.x occlusion culling and querying
    - opencl    - OpenCL generated texture rendering with GLES
    - drmkms    - DRM/KMS output for direct display rendering without compositor
  - vulkan
    - triangle  - Basic rendering without input attachment
    - texture   - Usage of Vulkan Image for texturing
    - yuv       - Texturing with various input/output YUV formats
    - compute   - Usage of compute shaders
  - opencl
    - fractal   - CL kernels for fractal image generation
    - scaling   - Gaussian/recursive filtering for imsgae upscaling/downscaling
    - edge      - Edge detection algorythm
    - histogram - Color histogram calculation


# Build

SDK application uses CMake as a build system and comes with
most of the build time dependencies through the Google repo.

Instructions below assume that path to the repo checkout is at `/path/to/src`.

## Host configuration

```bash
cmake \
    -G Ninja \
    /path/to/sdk
```

## Target configuration

Target build requires installed Yocto SDK with populated target sysroot.
Assuming Yocto SDK ins installed at `/path/to/yocto_sdk`.

```bash
XLNX_SDK=/path/to/yocto_sdk \
cmake \
    -G Ninja \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_TOOLCHAIN_FILE=xlnx.cmake \
    -D SDK_Platform=wayland \
    -D SDK_UseGLES3=YES \
    /path/to/sdk
```

## Compile

```bash
ninja sdk
```
