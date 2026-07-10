![AMD GPU Unified SDK](doc_imgs/SDK.png)

# AMD GPU Unified SDK

**A unified development toolkit for AMD Kria, Zynq UltraScale+, and Versal platforms — bringing the OpenGL ES, OpenCL, and Vulkan SDKs together in a single tree.**

[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](docs/html/index.html)
[![Platform](https://img.shields.io/badge/platform-AMD-red.svg)](https://www.amd.com)
[![OpenGL ES](https://img.shields.io/badge/OpenGL%20ES-2.0%20%7C%203.0-green.svg)](https://www.khronos.org/opengles/)
[![OpenCL](https://img.shields.io/badge/OpenCL-1.2%20%7C%202.0-green.svg)](https://www.khronos.org/opencl/)
[![Vulkan](https://img.shields.io/badge/Vulkan-1.0%20%7C%201.1-green.svg)](https://www.khronos.org/vulkan/)

## 🚀 **Overview**

The AMD GPU Unified SDK consolidates the three previously separate GPU SDKs —
**OpenGL ES**, **OpenCL**, and **Vulkan** — into one CMake project with shared
support libraries and a common build system. It targets AMD embedded platforms
including **Kria, Zynq UltraScale+, and Versal** systems, and
ships out-of-the-box samples that customers can use as a starting point for new
applications and to explore implementation possibilities.

### ✨ **Key Features**

- **🎯 Comprehensive Sample Suite**: Graphics (OpenGL ES / Vulkan) and compute
  (OpenCL) samples covering basic to advanced techniques.
- **🏗️ Shared SDK Libraries**: Common, GlesFramework, Vulkan, and OpenCL helper
  libraries reused across all sample sets.
- **🖥️ Multi-Platform Windowing**: Wayland, X11, and framebuffer (fbdev) backends
  selectable at configure time.
- **⚡ Performance Optimized**: Leverages Mali 400 / Mali G78AE GPU hardware acceleration.
- **🔧 Developer-Friendly**: One CMake configuration builds the whole tree.

## 📋 **Supported Platforms**

| Platform | Architecture | GPU | APIs | Status |
|----------|-------------|-----|------|--------|
| AMD Kria SOM | ARM64 | Mali 400 | OpenGL ES | ✅ Full Support (X11, FBDEV, Wayland)|
| Zynq UltraScale+ | ARM64 | Mali 400 | OpenGL ES | ✅ Full Support (X11, FBDEV, Wayland)|
| Versal AI Edge Series Gen 2 | ARM64 | Mali G78AE | OpenGL ES / OpenCL / Vulkan | ✅ Full Support (FBDEV, Wayland)|

---

# Project structure

- cmake
  - modules - Additional CMake module wrappers
    - ecm   - Extra CMake Modules (used in Wayland protocol compilation)
- 3rdparty - Bundled third-party code
  - stb    - stb_image.h single-header image loader (used by the Vulkan framework)
- lib
  - sdk
    - Common        - Graphics-API-agnostic helper classes
    - GlesFramework - OpenGL ES helper classes + windowing (Wayland/X11/fbdev), EGL, fonts
    - Vulkan        - Vulkan helper classes (RAII API)
    - OpenCL        - OpenCL context/kernel management layer
    - OpenGL        - DRM/DMA-BUF + EGL interop utilities (not built by default)
    - vulkan-strings    - Vulkan API identifiers to string convertor (debugging purposes)
    - vulkan-vendor-ids - GPU vendor name resolution from PCI ID global database (debugging purposes)
- samples
  - vulkan
    - basic-compute   - Compute-shader particle simulation rendered via graphics
    - hellotriangle   - Trivial triangle rendering
    - rotating-texture - Rotating textured quad (Vulkan image sampling)
    - spinning-cube   - Textured 3D cube with depth buffering
    - multipass       - Deferred shading with multiple subpasses
    - multisampling   - MSAA rendering
    - mipmapping      - Mipmap generation and sampling
    - astc            - ASTC compressed texture sampling (with PNG fallback)
    - multithreading  - Multi-threaded secondary command buffer recording

    All Vulkan samples select the windowing backend at runtime:
      --wayland  VK_KHR_wayland_surface (under a compositor such as Weston)
      --display  VK_KHR_display (direct DRM/KMS, no compositor)
      (no flag)  auto: wayland if a compositor socket is present, else display
  - opengles - OpenGL ES samples, grouped by GLES version. All samples build on
    the `SdkGlesFramework` (lib/sdk/GlesFramework) except the standalone
    FBDEV-only EGLImage and OpenCL demos noted below.
    - opengl_es_2.0
      - trianglesample               - Anti-aliased spinning triangle
      - alpha-depthsample            - Alpha blending and depth testing
      - bbox                         - Bounding-box rendering with on-screen text
      - eglpreserve                  - EGL_BUFFER_PRESERVED swap behaviour
      - eglrenderingsample           - EGLImage rendering (FBDEV only)
      - framebufferobject-fbosample  - Render-to-texture via an FBO
      - multicubesample              - Multiple textured spinning cubes
      - particlerenderingsample      - GPU particle / molecule system
      - rotozoom                     - Rotation/zoom texture effect
      - shadowsample                 - Shadow mapping
      - texturemipmapsample          - ETC mip-mapped textures
      - waveformsample               - Animated waveform rendering
      - yuv2rgbsample                - YUV-to-RGB colour conversion on the GPU
    - opengl_es_3.0
      - compute-particles            - Compute-shader particle system
      - occlusion-culling            - Hi-Z GPU occlusion culling
      - opencl-matmul                - OpenCL matrix multiply (built only when OpenCL is found)

    Each sample is collected into its own directory under the build output tree
    (`build/output/opengles/<backend>/<gles_version>/<sample>/`) with its
    shaders/assets staged alongside the binary: per-sample shaders in
    `Supportingfiles/`, the shared font shaders in `FontShaders/`, and any extra
    data (`models/`, `assets/`). The samples load these with paths relative to the
    working directory, so run each one from its own directory, after bringing up
    the backend's display server and sourcing its runtime-exports helper (see
    [Running the OpenGL ES samples](#running-the-opengl-es-samples)):

    ```bash
    cd build/output/opengles/wayland/opengl_es_2.0/trianglesample
    source <sdk>/samples/opengles/run_exports_wayland.sh
    ./trianglesample
    ```

    The windowing backend of the framework-based samples is selected at configure
    time with `-D SDK_Platform=<wayland|x11|fbdev>` (default `wayland`):
      - `wayland` - render through a Wayland (xdg-shell) surface; run under a
        compositor such as Weston.
      - `x11`     - render into an X11 window; run under an X server.
      - `fbdev`   - render directly to the Linux framebuffer (EGL_DEFAULT_DISPLAY);
        run from a bare console.
    Exactly one backend is compiled, and only that backend's system libraries are
    linked. `eglrenderingsample` is a standalone EGLImage demo that manages its
    own surface; it is **FBDEV only** (run from a bare console). It builds
    independently of `SDK_Platform` (its build links X11 for the standalone
    codebase, but the sample is intended to run on the framebuffer).
  - opencl
    - hello-world-c           - Array addition on the CPU only (no OpenCL, baseline reference)
    - hello-world-opencl      - Array addition in an OpenCL kernel (scalar)
    - vector-hello-world      - Array addition using vectorized OpenCL types
    - opencl-template         - Minimal skeleton that runs a no-argument kernel
    - mandelbrot              - Mandelbrot set generation written to a bitmap
    - mandelbrot-benchmark    - Mandelbrot generation with image perturbation and timing
    - matrix-multiply         - SGEMM benchmark suite (many kernel variants, CSV output)
    - sgemm                   - Single-precision matrix multiply (alpha*A*B + beta*C)
    - gaussian-filter         - Gaussian blur of a PNG image
    - convolution-filter      - User-defined 3x3/5x5 convolution filter
    - fir-filter              - FIR (finite impulse response) image filter
    - sobel-filter            - Sobel edge detection using vector loads (dX, dY, combined)
    - sobel-filter-no-vectors - Sobel edge detection, one pixel per work item
    - image-scaling           - Image upscaling using OpenCL image objects and a sampler
    - integer-64-bit          - Pixel sum/sum-of-squares using 64-bit ints and atomics
    - depth-map               - Stereo disparity / obstacle detection from two images
    - adas                    - Lane visualization via Sobel edge detection (ADAS)
    - ml-classifier           - Average-intensity feature + linear "Bright/Dark" classifier

    Most OpenCL samples read their inputs from `assets/` (relative to the
    executable) and write results (e.g. `output.bmp`/`output.png`) into the
    current working directory.


# Build

The SDK uses **CMake** (with the Ninja generator) as its build system. It does
not download anything at configure time: the build-time dependencies are either
vendored in-tree under `3rdparty/` (e.g. `stb_image`, the Vulkan headers) or
provided by the cross-compilation toolchain's sysroot (e.g. EGL/GLES, Vulkan,
Wayland, X11, zlib), located via `find_package`/`pkg-config`. The only external
prerequisite you must supply is that toolchain (the Yocto SDK), described under
[Target configuration](#target-configuration) below.

The instructions below assume you have already checked out the repository and are working from its root directory.

> **Important:** No sample sets are built by default. Add a platform preset
> (e.g. `-D SDK_PLATFORM_TARGET=versal-2ve-2vm`) or per-API switches
> (e.g. `-D SDK_BUILD_VULKAN=ON`) to your `cmake` command — see
> [Selecting what to build](#selecting-what-to-build) below.

## Target configuration

Target build requires installed Yocto SDK with populated target sysroot.
Assuming Yocto SDK installed at `/yocto/build/tmp/deploy/sdk/`.

Configure **out-of-source**: `-B build` is an explicit build directory. All generated CMake files (the cache, `CMakeFiles/`, `build.ninja`, etc.) are written under `build/`, keeping the source tree clean.

```bash
source /yocto/build/tmp/deploy/sdk/environment-setup-cortexa72-cortexa53-amd-linux

export AMD_SDK=/yocto/build/tmp/deploy/sdk/

cmake \
    -G Ninja \
    -B build \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_TOOLCHAIN_FILE=sdk.cmake \
    -D SDK_Platform=<x11/fbdev/wayland> \
    -D SDK_PLATFORM_TARGET=<versal-2ve-2vm/zynqmp> \
```

`SDK_Platform` selects the OpenGL ES samples' windowing backend
(`wayland` (default) | `x11` | `fbdev`); see the `samples/opengles` notes above.
`SDK_PLATFORM_TARGET` selects *which* sample sets are built (nothing by default);
see [Selecting what to build](#selecting-what-to-build) below.

## Selecting what to build

**Nothing is built by default** — you must opt in to the sample sets you want,
either with a target platform preset or with the explicit per-API switches.

### Target platform preset

`SDK_PLATFORM_TARGET` enables a set of sample sets based on the SoC's GPU
capabilities:

| `SDK_PLATFORM_TARGET` | GPU | Sample sets enabled |
|-----------------------|-----|---------------------|
| `none` (default) | — | nothing (use the per-API switches) |
| `versal-2ve-2vm` | Mali G78AE | OpenGL ES 2.0 + 3.0, OpenCL, Vulkan |
| `zynqmp` | Mali 400 | OpenGL ES 2.0 only |

```bash
# Build everything (Versal AI Edge Series Gen 2 / Mali G78AE) examples
export AMD_SDK=/path/to/yocto_sdk/
cmake -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_TOOLCHAIN_FILE=sdk.cmake -D SDK_Platform=<fbdev/wayland> -D SDK_PLATFORM_TARGET=versal-2ve-2vm -B build -D ...

# Build only the (ZynqMP / Mali 400) examples -> OpenGL ES 2.0 only
export AMD_SDK=/path/to/yocto_sdk/
cmake -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_TOOLCHAIN_FILE=sdk.cmake -D SDK_Platform=<x11,fbdev,wayland> -D SDK_PLATFORM_TARGET=zynqmp -B build -D ...
```

Each sample's application binary and its supporting files are collected under the
build directory (OpenGL ES: `build/output/opengles/<backend>/<gles_version>/<sample>/`;
OpenCL/Vulkan: `build/output/<api>/<sample>/`; see [Compile](#compile)), not inside
the source tree.

### Per-API switches

Each option's **default is derived from `SDK_PLATFORM_TARGET`** (all OFF for the
default `none`; see the table above). An explicit `-D SDK_BUILD_*` on the command
line overrides the platform-preset default, so any combination can be built:

| Option | Default (`none` / `zynqmp` / `versal-2ve-2vm`) | Effect |
|--------|------------------------------------------------|--------|
| `SDK_BUILD_OPENGLES` | OFF / ON / ON | Build the OpenGL ES samples |
| `SDK_BUILD_OPENGLES_2_0` | OFF / ON / ON | Build the OpenGL ES **2.0** samples |
| `SDK_BUILD_OPENGLES_3_0` | OFF / OFF / ON | Build the OpenGL ES **3.0** samples |
| `SDK_BUILD_OPENCL` | OFF / OFF / ON | Build the OpenCL samples |
| `SDK_BUILD_VULKAN` | OFF / OFF / ON | Build the Vulkan samples |

> Enabling `SDK_BUILD_OPENGLES` without selecting a version builds **both** 2.0
> and 3.0; use `SDK_BUILD_OPENGLES_2_0` / `_3_0` to pick one.

```bash
# Build only the OpenCL samples
export AMD_SDK=/path/to/yocto_sdk/
cmake -G Ninja ... \
    -D SDK_BUILD_OPENGLES=OFF -D SDK_BUILD_OPENCL=ON -D SDK_BUILD_VULKAN=OFF \
    -D ...

# Build only the Vulkan samples
export AMD_SDK=/path/to/yocto_sdk/
cmake -G Ninja ... \
    -D SDK_BUILD_OPENGLES=OFF -D SDK_BUILD_OPENCL=OFF -D SDK_BUILD_VULKAN=ON \
    -D ...

# Build only the OpenGL ES samples (both versions for versal-2ve-2vm and OpenGLES 2.0 for zynqmp)
export AMD_SDK=/path/to/yocto_sdk/
cmake -G Ninja ... \
    -D SDK_BUILD_OPENGLES=ON -D SDK_BUILD_OPENCL=OFF -D SDK_BUILD_VULKAN=OFF \
    -D ...
```

Only the support libraries for the enabled sample sets are built, so a subset
build does not require the toolchain packages of the disabled APIs (e.g. a
ZynqMP / OpenGL-ES-only build needs neither the Vulkan nor OpenCL SDK packages).

> **Note:** Because these are cached CMake options, switching
> `SDK_PLATFORM_TARGET` after a first configure does not retroactively change the
> per-API option values. To switch platforms cleanly, configure into a fresh
> build directory, or pass the `SDK_BUILD_*` switches explicitly.

## Compile

Build from the explicit build directory configured above:

```bash
ninja -C build
```

All build artifacts stay under `build/output/`. The sample binaries and their runtime
input/asset files are collected under `build/output/`, classified by API:

```
build/output/
├── lib/                                          # SDK shared libs (libSdkCommon/OpenCL/Vulkan)
├── opengles/<backend>/opengl_es_2.0/<sample>/    # <backend> = wayland | x11 | fbdev
├── opengles/<backend>/opengl_es_3.0/<sample>/
├── opencl/<sample>/
└── vulkan/<sample>/
```

> The collection folder name defaults to `output` under the build directory.
> Override it (e.g. to a path outside the tree) with
> `-D SDK_OUTPUT_DIR=/abs/path/out` at configure time.

> The OpenCL and Vulkan samples link the SDK shared libraries, which are staged
> once into the single `build/output/lib/` directory. Each such sample carries an
> `$ORIGIN/../../lib` RPATH, so it locates those libraries automatically -- no
> `LD_LIBRARY_PATH` is needed. When deploying to a target, keep the shared `lib/`
> folder at the same level relative to the sample (i.e. copy `build/output/` as a
> unit, or preserve the `<api>/<sample>` + sibling `lib/` layout). The OpenGL ES
> samples link the framework statically and need no shared libraries.

# Running the OpenCL samples

Each OpenCL sample is collected into its own directory under the build output
tree (`build/output/opencl/<sample>/`) together with its `assets/` directory
(kernels and input images). Because the samples load their kernels and inputs
with paths relative to the working directory (e.g. `assets/foo.cl`,
`assets/input.bmp`), run each sample from inside its own directory:

```bash
cd build/output/opencl/gaussian-filter
./gaussian_filter
```

Notes:

- Outputs are written to the current directory: filter/image samples produce
  `output.bmp` or `output.png` (the Sobel samples additionally write
  `output-dX.bmp` and `output-dY.bmp`).
- Samples print OpenCL kernel profiling information (queued/wait/run time) on
  completion.

Sample-specific inputs and arguments:

- `matrix-multiply` accepts command line options (run `./matrix_multiply -h` for
  the full list, including `-va` to validate against a CPU reference and
  `-csv <file>` to dump results).
- `mandelbrot-benchmark` and `adas` expect a 1024x1024
  `assets/input.bmp`.
- `depth-map` expects a stereo pair `assets/left.bmp` and `assets/right.bmp`.
- `sobel-filter` expects an `assets/input.bmp` whose width is a multiple of 16.
- Image samples read `assets/input.bmp`.
- `integer-64-bit` reads `assets/input.bmp`, prints the pixel sum and
  sum-of-squares to the console (no output file), and requires the
  `cl_khr_int64_base_atomics` device extension.
- `hello-world-c`, `hello-world-opencl`, `vector-hello-world`, `opencl-template`
  and `sgemm` take no inputs and produce no image output.


# Running the OpenGL ES samples

The windowing backend is chosen at configure time with
`-D SDK_Platform=<wayland|x11|fbdev>` (default `wayland`). At run time each
sample needs its backend's display server up and the matching environment
exported.

A per-backend runtime-exports helper is provided under `samples/opengles/`:

```
run_exports_wayland.sh   run_exports_X11.sh   run_exports_fbdev.sh
```

Each helper sets `DISPLAY` and the backend-specific display vars (for Wayland,
the compositor-socket discovery).

> **Important:** you must **`source`** the helper (so its `export`s apply to your
> current shell) -- do **not** run it with `./` (a child process cannot change
> your shell's environment).

> **ZynqMP only -- select the Mali driver variant.** On ZynqMP the Mali
> user-space driver ships per-backend variants
> (`/usr/lib/<backend>/libMali.so.9.0`); the active one **must** match the
> windowing backend the samples were built for, otherwise EGL fails with
> `No EGL Display available`. Select it once (as **root**) before running:
>
> ```bash
> # <backend> = wayland | x11 | fbdev
> update-alternatives --install /usr/lib/libMali.so.9.0 libmali /usr/lib/<backend>/libMali.so.9.0 30
> ```
>
> This is **not** needed on Versal 2VE/2VM.

Because every sample loads its shaders/assets with paths relative to the working
directory, run each one **from inside its own directory**.

- **`wayland`** -- render through a Wayland (xdg-shell) surface.

  1. Start the compositor if it is not already running. On the AMD Yocto image
     Weston is a systemd service:

     ```bash
     systemctl start weston            # start Weston
     systemctl status weston           # confirm it is active
     ```

     Check whether one is already up with
     `ls -l /run/wayland-* /run/user/*/wayland-*`.
  2. Source the helper and run the sample:

     ```bash
     cd build/output/opengles/wayland/opengl_es_2.0/trianglesample
     source <sdk>/samples/opengles/run_exports_wayland.sh
     ./trianglesample
     ```

     The helper discovers the compositor socket via
     `XDG_RUNTIME_DIR`/`WAYLAND_DISPLAY`. If the compositor runs under a
     different user than the one launching the sample, override them after
     sourcing:

     ```bash
     export XDG_RUNTIME_DIR=/run/user/1000   # the compositor user's runtime dir
     export WAYLAND_DISPLAY=wayland-1         # the socket name in that dir
     ```

- **`x11`** -- render into an X11 window.

  1. Start the X server if it is not already running. On the AMD Yocto image the
     X server is a systemd service:

     ```bash
     systemctl start xserver-nodm      # start the X server
     systemctl status xserver-nodm     # confirm it is active
     ```

     Alternatively, start the Xorg server directly:

     ```bash
     /usr/bin/Xorg -depth 16 &
     ```
  2. Source the helper (it exports `DISPLAY=:0.0`) and run the sample:

     ```bash
     cd build/output/opengles/x11/opengl_es_2.0/trianglesample
     source <sdk>/samples/opengles/run_exports_X11.sh
     ./trianglesample
     ```

- **`fbdev`** -- render directly to the Linux framebuffer (`/dev/fb0`). Run from
  a bare console with **no** compositor holding the display:

  ```bash
  cd build/output/opengles/fbdev/opengl_es_2.0/trianglesample
  source <sdk>/samples/opengles/run_exports_fbdev.sh
  ./trianglesample
  ```

  **Fullscreen / render resolution.** The samples render at a fixed size
  (default 1920x1080). For a full-screen result, set the render size to match your display's resolution at configure time using
  the `GLES_WINDOW_W` / `GLES_WINDOW_H` CMake variables, e.g.:

  ```bash
  cmake ... -D SDK_Platform=fbdev -D GLES_WINDOW_W=3840 -D GLES_WINDOW_H=2160
  ```

  This applies to all framework-based OpenGL ES samples (it sets the render
  surface, viewport, and projection size). Match the values to your panel's
  native resolution for a full-screen render.

> **`eglrenderingsample` is different:** it is a standalone **FBDEV-only** demo
> that manages its own EGL surface via `/dev/fb0` and does **not** use the
> framework's windowing. It ignores the Wayland/X11 backends -- run it from a
> bare console with a working framebuffer, regardless of the `SDK_Platform` the
> rest of the build used. (Even though it is staged under the `<backend>` folder
> named after your `SDK_Platform`, that folder name is only a label; the binary
> is always the fbdev demo.)


# Running the Vulkan samples

Unlike the OpenGL ES samples, the Vulkan samples select their windowing backend
**at run time** (a single binary supports both), so there is no `SDK_Platform`
build switch or exports helper for them. Each sample is collected into its own
directory with its compiled SPIR-V shaders and textures staged alongside:

```
build/output/vulkan/<sample>/            # binary + assets/shaders/*.spv (+ assets/textures)
```

Because the samples load their assets with paths relative to the working
directory, run each one **from inside its own directory**:

```bash
cd build/output/vulkan/hellotriangle
./hellotriangle            # auto-detect backend
```

**Windowing backend selection** -- pass a flag, or let it auto-detect:

| Flag | Backend | When to use |
|---|---|---|
| `--wayland` | `VK_KHR_wayland_surface` | a compositor (e.g. Weston) is running |
| `--display` | `VK_KHR_display` (direct DRM/KMS) | no compositor; the sample owns the display |
| *(none)* | auto | wayland if a compositor socket is present, else display |

- **`--wayland`** -- start the compositor first and point the client at its
  socket (same as the OpenGL ES Wayland steps):

  ```bash
  systemctl start weston
  export XDG_RUNTIME_DIR=/run/user/1000    # only if the compositor runs as another user
  export WAYLAND_DISPLAY=wayland-1          #   (find it: ls -l /run/user/*/wayland-*)
  cd build/output/vulkan/spinning-cube
  ./spinning-cube --wayland
  ```

- **`--display`** -- run from a bare console with **no** compositor holding the
  display (`VK_KHR_display` needs exclusive DRM/KMS access):

  ```bash
  cd build/output/vulkan/spinning-cube
  ./spinning-cube --display
  ```

  > If a compositor is running it owns the display and `--display` reports
  > "No displays available." -- stop the compositor, or use `--wayland` instead.

The `basic-compute` sample runs a compute-shader simulation and presents the
result the same way (it accepts the same backend flags).


---

# 🎨 Complete Sample Reference

> **Note on images:** The snapshots below reference artwork under
> `doc_imgs/opengles/`, `doc_imgs/opencl/`, and `doc_imgs/vulkan/`. Drop the
> corresponding image files into the matching folder to have them render here.

## OpenGL ES Samples

### **OpenGL ES 2.0 Samples**

#### 1. **Triangle Sample**
![Triangle Sample](doc_imgs/opengles/TriangleSample.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/trianglesample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** A basic triangle rendered in OpenGL ES 2.0 with color
interpolation across vertices and rotation matrices for dynamic animation. A
perfect starting point for learning OpenGL ES fundamentals.

**Key Techniques:** Vertex shaders, color interpolation, basic rendering pipeline, matrix transformations

---

#### 2. **MultiCube Sample**
![MultiCube Sample](doc_imgs/opengles/cube.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/multicubesample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates basic cube rendering with color interpolation and
rotation matrices, and the rendering of multiple objects. Essential for
understanding 3D transformations.

**Key Techniques:** 3D graphics, matrix operations, depth testing, multiple object rendering

---

#### 3. **Texture MipMap Sample**
![MipMap Sample](doc_imgs/opengles/mipmap.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/texturemipmapsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Shows the Mali GPU decompressing a compressed texture on the fly
with multiple MIPMAP levels used as textures (stored as compressed ETC files).

**Key Techniques:** ETC1 compression, mipmap generation, texture optimization, GPU decompression

---

#### 4. **Alpha Depth Sample**
![Alpha Sample](doc_imgs/opengles/alpha.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/alpha-depthsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates transparency rendering and depth projection effects
with proper alpha blending and depth testing.

**Key Techniques:** Alpha blending, depth projection, transparency effects, render order management

---

#### 5. **Frame Buffer Object (FBO) Sample**
![FBO Sample](doc_imgs/opengles/FBO.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/framebufferobject-fbosample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Render-to-texture using a framebuffer object: a colored cube is
rendered to a texture and applied across the faces of an outer cube, with both
cubes animated by rotation matrices.

**Key Techniques:** Render-to-texture, FBO management, multi-pass rendering, texture as render target

---

#### 6. **Particle Rendering Sample**
![Particle Sample](doc_imgs/opengles/particle.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/particlerenderingsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Particle effect simulation on the Mali GPU. Draws many particles
created from a small texture, each with its own lifetime and color, animated via
matrices.

**Key Techniques:** Point sprites, physics simulation, dynamic particle management, GPU-based effects

---

#### 7. **Shadow Mapping Sample**
![Shadow Sample](doc_imgs/opengles/ShadowSample.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/shadowsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Showcases lighting and shadow effects on the Mali 400 GPU with
real-time shadow rendering techniques.

**Key Techniques:** Shadow maps, depth textures, multi-pass rendering, advanced lighting

---

#### 8. **Waveform Pattern Sample**
![Wave Sample](doc_imgs/opengles/wave.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/waveformsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Waveform rendering using only GLSL shader code while minimizing
vertices; multiple waveforms are implemented with math functions in the pixel
shader.

**Key Techniques:** Vertex displacement, procedural animation, mathematical functions in shaders

---

#### 9. **RotoZoom Sample**
![RotoZoom Sample](doc_imgs/opengles/roto.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/rotozoom/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Classic demo effect showcasing 2D transformations and texture
sampling with rotation and zoom.

**Key Techniques:** 2D transformations, texture sampling, classic demo effects

---

#### 10. **YUV2RGB Conversion Sample**

**📁 Source**: `samples/opengles/opengl_es_2.0/yuv2rgbsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Fast color conversion from YUV to RGB in a GLSL shader on the
fly, rendering the converted image to the framebuffer frame by frame.

**Key Techniques:** Color space conversion, video textures, GPU-based color conversion

**Note:** *You must provide your own raw YUV (yuv420p) frame and pass it as the
first argument (e.g. `./yuv2rgbsample input.yuv`). Run the sample with `-h` for
usage details, including an `ffmpeg` command to generate a compatible file.*

---

#### 11. **EGL Image Texture Swapping Sample**
![EGL Sample](doc_imgs/opengles/EGL1.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/eglrenderingsample/`

**Supported Window Environments:** ✅ FBDEV only

**Description:** Showcases the EGL Image extension, which defines an EGL resource
type (EGLImage) suitable for sharing 2D arrays of image data between client APIs.

**Key Techniques:** EGL Image extension, texture sharing, EGL resource management

---

#### 12. **EGL Preserve Sample**
![EGL Preserve Sample](doc_imgs/opengles/EGL_preserve.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/eglpreserve/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates EGL buffer preservation (`EGL_BUFFER_PRESERVED`)
and context management for efficient rendering and resource management.

**Key Techniques:** EGL context preservation, buffer management, efficient rendering

---

#### 13. **BBOX Sample**
![BBOX Sample](doc_imgs/opengles/bbox.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/bbox/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates bounding-box rendering, reading the bbox
coordinates and class label from a text file in the same directory.

**Key Techniques:** BBOX rendering, on-screen text

---

### **OpenGL ES 3.0 Samples**

#### 1. **Compute Particles Sample**
![Compute Particles Sample](doc_imgs/opengles/compute_particles.png)

**📁 Source**: `samples/opengles/opengl_es_3.0/compute-particles/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** GPU compute-shader particle system using modern OpenGL ES 3.x
compute capabilities for high-performance particle simulation.

**Key Techniques:** GPU compute shaders, SSBO, compute dispatch, memory barriers, parallel processing

---

#### 2. **Occlusion Culling Sample**
![Occlusion Culling Sample](doc_imgs/opengles/occlusion_culling.png)

**📁 Source**: `samples/opengles/opengl_es_3.0/occlusion-culling/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Visibility optimization using Hi-Z GPU occlusion culling for
improved rendering performance in complex scenes.

**Key Techniques:** Hi-Z occlusion, indirect rendering, visibility optimization

---

#### 3. **OpenCL Matrix Multiplication Sample**

**📁 Source**: `samples/opengles/opengl_es_3.0/opencl-matmul/`

**Description:** Demonstrates GPGPU computing using OpenCL for high-performance
matrix operations with OpenGL ES interoperability (built only when OpenCL is
found).

**Key Techniques:** GPGPU computing, OpenCL interop, matrix operations, parallel computing

---

## OpenCL Samples

#### 1. **Hello World OpenCL**

**📁 Source**: `samples/opencl/hello-world-opencl/`

**Description:** Basic integer array addition implemented in an OpenCL kernel. Produces
no image or result file; it only prints kernel profiling information on completion,
confirming that an OpenCL device is available and the kernel ran.

**Key Techniques:** OpenCL initialization, kernel creation, buffer management, basic parallel computation

---

#### 2. **Hello World C**

**📁 Source**: `samples/opencl/hello-world-c/`

**Description:** CPU-only baseline that performs the same integer array addition in
plain C/C++ (no OpenCL). It prints nothing and writes no file — if it runs and exits
successfully, your C/C++ toolchain works.

**Key Techniques:** C-style OpenCL API usage, context creation, simple kernel execution

---

#### 3. **Mandelbrot Set**
![Mandelbrot Sample](doc_imgs/opencl/Mandelbrot.bmp)

**📁 Source**: `samples/opencl/mandelbrot/`

**Description:** Generates Mandelbrot fractal data, testing each pixel to
determine the iterations needed to rule out set membership, output as a grayscale
bitmap (`output.bmp`).

**Key Techniques:** Parallel fractal computation, 2D indexing, mathematical computation on GPU

---

#### 4. **Mandelbrot Benchmark**
![Mandelbrot Benchmark Sample](doc_imgs/opencl/Mandelbrot_bench.bmp)

**📁 Source**: `samples/opencl/mandelbrot-benchmark/`

**Description:** Performance-focused Mandelbrot generator with timing and image
perturbation for benchmarking GPU compute. Expects a 1024x1024 `assets/input.bmp`.

**Key Techniques:** Performance measurement, GPU benchmarking, optimized parallel computation

---

#### 5. **Image Scaling**
![Image Scaling Sample](doc_imgs/opencl/Imagescaling.bmp)

**📁 Source**: `samples/opencl/image-scaling/`

**Description:** Uses OpenCL image objects and a sampler to resize images on the
GPU.

**Key Techniques:** OpenCL image objects, 2D image processing, bilinear interpolation, image resampling

---

#### 6. **Sobel Filter**
![Sobel Filter Sample](doc_imgs/opencl/Sobel_filter.bmp)

**📁 Source**: `samples/opencl/sobel-filter/`

**Description:** Edge detection using the Sobel operator with vector loads,
computing X and Y gradients and combining them. Expects an `assets/input.bmp`
whose width is a multiple of 16.

**Key Techniques:** Edge detection, convolution kernels, gradient computation, vector loads

---

#### 7. **Sobel Filter (No Vectors)**
![Sobel No Vectors Sample](doc_imgs/opencl/Sobel_filter_no_vec.bmp)

**📁 Source**: `samples/opencl/sobel-filter-no-vectors/`

**Description:** Sobel edge detection without vector types — one pixel per work
item — demonstrating an alternative optimization approach.

**Key Techniques:** Scalar-based image processing, alternative optimization strategies, edge detection

---

#### 8. **Gaussian Filter**
![Gaussian Filter Sample](doc_imgs/opencl/Gaussianfilter.png)

**📁 Source**: `samples/opencl/gaussian-filter/`

**Description:** Applies a Gaussian blur to a PNG image (`assets/input.png`) for
noise reduction and smoothing.

**Key Techniques:** Gaussian blur, separable convolution, image smoothing, noise reduction

---

#### 9. **Convolution Filter**
![Convolution Filter Sample](doc_imgs/opencl/Convolutionfilter.bmp)

**📁 Source**: `samples/opencl/convolution-filter/`

**Description:** General-purpose 3x3/5x5 convolution filtering framework
supporting various kernel types.

**Key Techniques:** General convolution, custom kernels, flexible image processing framework

---

#### 10. **FIR Filter**
![FIR Filter Sample](doc_imgs/opencl/Firfilter.bmp)

**📁 Source**: `samples/opencl/fir-filter/`

**Description:** Finite Impulse Response filtering for signal and image
processing on the GPU.

**Key Techniques:** FIR filtering, signal processing, digital filter implementation

---

#### 11. **Depth Map**
![Depth Map Sample](doc_imgs/opencl/Depthmap.bmp)

**📁 Source**: `samples/opencl/depth-map/`

**Description:** Stereo-vision depth map generation for obstacle detection.
Expects a stereo pair `assets/left.bmp` and `assets/right.bmp`.

**Key Techniques:** Stereo vision, depth estimation, correlation algorithms, 3D reconstruction

---

#### 12. **ML Classifier**
![ML Classifier Sample](doc_imgs/opencl/ML_classifier.bmp)

**📁 Source**: `samples/opencl/ml-classifier/`

**Description:** Simple ML image classifier that computes average intensity and
applies a linear classifier to label images as "Bright" or "Dark".

**Key Techniques:** Machine learning, image classification, statistical analysis, feature extraction

---

#### 13. **Matrix Multiply**

**📁 Source**: `samples/opencl/matrix-multiply/`

**Description:** High-performance SGEMM benchmark suite with many kernel variants
(local memory, work-group optimization, tiling). Run `./matrix_multiply -h` for
options including `-va` (validate vs. CPU) and `-csv <file>`.

**Key Techniques:** Matrix operations, local memory optimization, work-group coordination, linear algebra

---

#### 14. **Integer 64-bit**

**📁 Source**: `samples/opencl/integer-64-bit/`

**Description:** Computes the sum and the sum-of-squares of an image's pixel
(luminance) values using 64-bit integers and atomics, then prints both totals to
the console (no image or result file is written). Reads `assets/input.bmp` and
requires the `cl_khr_int64_base_atomics` device extension.

**Key Techniques:** 64-bit arithmetic, extended precision, atomics

---

#### 15. **SGEMM**

**📁 Source**: `samples/opencl/sgemm/`

**Description:** Single-precision general matrix multiply (`alpha*A*B + beta*C`)
showcasing optimized linear algebra operations.

**Key Techniques:** BLAS operations, optimized matrix multiplication, performance tuning

---

#### 16. **Vector Hello World**

**📁 Source**: `samples/opencl/vector-hello-world/`

**Description:** Introduction to vector operations in OpenCL, demonstrating SIMD
programming patterns and vector data types.

**Key Techniques:** Vector operations, SIMD programming, parallel data processing

---

#### 17. **OpenCL Template**

**📁 Source**: `samples/opencl/opencl-template/`

**Description:** Minimal boilerplate skeleton for creating new OpenCL
applications with standard setup and error handling.

**Key Techniques:** Project template, best practices, error handling, code organization

---

#### 18. **ADAS Lane Detection**
![ADAS Lane Detection Sample](doc_imgs/opencl/Adas.bmp)

**📁 Source**: `samples/opencl/adas/`

**Description:** Lane visualization via Sobel edge detection (ADAS use case).
Expects a 1024x1024 `assets/input.bmp`.

**Key Techniques:** Edge detection, ADAS, image processing

---

## Vulkan Samples

#### 1. **Hello Triangle**
![Hello Triangle Sample](doc_imgs/vulkan/hello_triangle.png)

**📁 Source**: `samples/vulkan/hellotriangle/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Minimal Vulkan sample that renders a single triangle.
Demonstrates swapchain setup, render passes, and the basic graphics pipeline.

**Key Techniques:** Vulkan initialization, swapchain, render pass, graphics pipeline, vertex buffers

---

#### 2. **Rotating Texture**
![Rotating Texture Sample](doc_imgs/vulkan/rotating_texture.gif)

**📁 Source**: `samples/vulkan/rotating-texture/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Renders a textured quad and animates its rotation. Demonstrates
texture loading, samplers, and uniform buffer updates.

**Key Techniques:** Texture loading, samplers, uniform buffers, animation, transformation matrices

---

#### 3. **Basic Compute**
![Compute Sample](doc_imgs/vulkan/basic_compute.gif)

**📁 Source**: `samples/vulkan/basic-compute/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Compute-shader particle simulation rendered via graphics,
demonstrating compute-to-graphics synchronization and storage buffers.

**Key Techniques:** Compute shaders, storage buffers, memory barriers, dispatch, particle simulation

---

#### 4. **Multithreading**
![Multithreading Sample](doc_imgs/vulkan/multithreading.gif)

**📁 Source**: `samples/vulkan/multithreading/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Multi-threaded secondary command buffer recording and submission
for improved CPU-side performance.

**Key Techniques:** Multithreaded command buffers, synchronization, parallel rendering

---

#### 5. **Multisampling**
![Multisampling Sample](doc_imgs/vulkan/multisampling.gif)

**📁 Source**: `samples/vulkan/multisampling/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Enables and uses multisample anti-aliasing (MSAA) for smoother
rendering.

**Key Techniques:** Multisample anti-aliasing, render pass setup, resolve attachments

---

#### 6. **Spinning Cube**
![Spinning Cube Sample](doc_imgs/vulkan/spinning_cube.gif)

**📁 Source**: `samples/vulkan/spinning-cube/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Renders a textured 3D cube with rotation animation and depth
buffering.

**Key Techniques:** 3D transformations, depth buffer, animation, uniform buffers

---

#### 7. **Multi-pass Rendering**
![Multi-pass Sample](doc_imgs/vulkan/multipass.gif)

**📁 Source**: `samples/vulkan/multipass/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Deferred shading using multiple subpasses and framebuffers,
useful for post-processing and advanced rendering effects.

**Key Techniques:** Render passes, subpasses, framebuffers, image attachments, post-processing

---

#### 8. **ASTC Texture Compression**
![ASTC Sample](doc_imgs/vulkan/astc.gif)

**📁 Source**: `samples/vulkan/astc/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Loads and displays ASTC compressed textures, with fallback to
PNG if ASTC is not supported.

**Key Techniques:** ASTC texture loading, compressed textures, format fallback

---

#### 9. **Mipmapping**
![Mipmapping Sample](doc_imgs/vulkan/mipmapping_manually_generated.gif)

**📁 Source**: `samples/vulkan/mipmapping/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Shows the use and visual effect of mipmaps, demonstrating both
pre-generated and GPU-generated mipmaps.

**Key Techniques:** Mipmaps, texture sampling, image blitting, LOD selection

---

# 🏗️ Framework Architecture

The unified SDK provides shared support libraries under `lib/sdk/`, reused by all
sample sets:

- **Common** (`SdkCommon`) — Graphics-API-agnostic helpers: argument parsing,
  math/geometry, timing and frame counting, image/bitmap I/O, file/folder
  utilities, and DMA/GBM buffer helpers.
- **GlesFramework** (`SdkGlesFramework`) — OpenGL ES helper classes and the
  windowing abstraction (Wayland / X11 / fbdev), including EGL context/surface
  management, font rendering, and texture and matrix utilities.
- **Vulkan** (`SdkVulkan`) — RAII-style Vulkan helper classes (context,
  swapchain, command buffers, assets) backing the Vulkan samples.
- **vulkan-strings / vulkan-vendor-ids** (`VulkanStrings` / `VulkanVendorIds`) —
  Debugging helpers that resolve Vulkan enums and GPU vendor names to
  human-readable strings.
- **OpenCL** (`SdkOpenCL`) — A thin OpenCL context/kernel management layer used by
  the OpenCL samples (built on the shared `Common` image/bitmap helpers).
- **OpenGL** (`SdkOpenGL`) — DRM/DMA-BUF and EGL interop utilities for desktop
  OpenGL / zero-copy texturing (not built in the default configuration).

# 🔧 Adding New Samples

The build system is designed to be general. To add a new sample, copy an existing
one in the relevant `samples/<api>/` directory, rename the target in its
`CMakeLists.txt`, and add an `add_subdirectory(<newsample>)` to the parent
`CMakeLists.txt`. Place source files in the sample folder, shaders/kernels and
data under the sample's `Supportingfiles/`, `shaders/`, `assets/`, or
`cl_kernels/` directory as appropriate.

# 📄 License

This project is licensed under the terms in the [LICENSE](LICENSE) file.
