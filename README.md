![AMD GPU Unified SDK](doc_imgs/SDK_OpenGLES.png)

# AMD GPU Unified SDK

**A unified development toolkit for AMD Kria, Zynq UltraScale+, Versal, and Ryzen Embedded platforms — bringing the OpenGL ES, OpenCL, and Vulkan SDKs together in a single tree.**

[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](docs/html/index.html)
[![Platform](https://img.shields.io/badge/platform-AMD-red.svg)](https://www.amd.com)
[![OpenGL ES](https://img.shields.io/badge/OpenGL%20ES-2.0%20%7C%203.0-green.svg)](https://www.khronos.org/opengles/)
[![OpenCL](https://img.shields.io/badge/OpenCL-1.2%20%7C%202.0-green.svg)](https://www.khronos.org/opencl/)
[![Vulkan](https://img.shields.io/badge/Vulkan-1.0%20%7C%201.1-green.svg)](https://www.khronos.org/vulkan/)

## 🚀 **Overview**

The AMD GPU Unified SDK consolidates the three previously separate GPU SDKs —
**OpenGL ES**, **OpenCL**, and **Vulkan** — into one CMake project with shared
support libraries and a common build system. It targets AMD embedded platforms
including **Kria, Zynq UltraScale+, Versal, and Ryzen Embedded** systems, and
ships out-of-the-box samples that customers can use as a starting point for new
applications and to explore implementation possibilities.

### ✨ **Key Features**

- **🎯 Comprehensive Sample Suite**: Graphics (OpenGL ES / Vulkan) and compute
  (OpenCL) samples covering basic to advanced techniques.
- **🏗️ Shared SDK Libraries**: Common, EGL, GLES, and Vulkan helper libraries
  reused across all sample sets.
- **🖥️ Multi-Platform Windowing**: Wayland, X11, and framebuffer (fbdev) backends
  selectable at configure time.
- **⚡ Performance Optimized**: Leverages Mali 400 / Mali G78AE and AMD Radeon
  GPU hardware acceleration.
- **🔧 Developer-Friendly**: One CMake configuration builds the whole tree.

## 📋 **Supported Platforms**

| Platform | Architecture | GPU | APIs | Status |
|----------|-------------|-----|------|--------|
| AMD Kria SOM | ARM64 | Mali 400 | OpenGL ES | ✅ Full Support (X11, FBDEV, Wayland)|
| Zynq UltraScale+ | ARM64 | Mali 400 | OpenGL ES | ✅ Full Support (X11, FBDEV, Wayland)|
| Versal ACAP | ARM64 | Mali G78AE | OpenGL ES / OpenCL / Vulkan | ✅ Full Support (FBDEV, Wayland)|
| Ryzen Embedded | x86_64 | Radeon | OpenCL / Vulkan | ✅ Full Support (X11, Wayland)|
| x86 Development | x86_64 | Radeon | OpenGL ES / OpenCL / Vulkan | ✅ Development Support (X11, Wayland) |

---

# Project structure

- cmake
  - modules - Additional CMake module wrappers
    - ecm   - Extra CMake Modules (used in Wayland protocol compilation)
- 3rdparty - Bundled third-party code
  - stb    - stb_image.h single-header image loader (used by the Vulkan framework)
- lib
  - sdk
    - common - Common graphical API agnostic helper classes
    - egl    - EGL helper classes
    - gles   - GLES helper classes
    - vulkan - Vulkan helper classes (RAII API)
    - vulkan-strings    - Vulkan API identifiers to string convertor (debugging purposes)
    - vulkan-vendor-ids - GPU vendor name resolution from PCI ID global database (debugging purposes)
- samples
  - vulkan
    - basic-compute   - Compute-shader particle simulation rendered via graphics
    - triangle        - Trivial triangle rendering
    - texture         - Rotating textured quad (Vulkan image sampling)
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
  - opengles - OpenGL ES samples ported from the legacy GPU_SDK_OpenGLES, kept
    in their original GLES-version grouping (folder/target names lowercased).
    All samples build on the vendored `SdkGlesFramework` (lib/sdk/GlesFramework)
    except the standalone X11/fbdev and OpenCL demos noted below.
    - opengl_es_2.0
      - trianglesample               - Anti-aliased spinning triangle
      - alpha_depthsample            - Alpha blending and depth testing
      - bbox                         - Bounding-box rendering with on-screen text
      - eglpreserve                  - EGL_BUFFER_PRESERVED swap behaviour
      - eglrenderingsample           - EGLImage rendering (standalone X11/fbdev)
      - egltexturevideosample        - EGLImage external texture / video (standalone X11/fbdev)
      - framebufferobject-fbosample  - Render-to-texture via an FBO
      - meshrendersample             - OBJ/3DS mesh loading and lighting
      - multicubesample              - Multiple textured spinning cubes
      - particlerenderingsample      - GPU particle / molecule system
      - rotozoom                     - Rotation/zoom texture effect
      - shadowsample                 - Shadow mapping
      - texturemipmapsample          - ETC mip-mapped textures
      - waveformsample               - Animated waveform rendering
      - yuv2rgbsample                - YUV-to-RGB colour conversion on the GPU
    - opengl_es_3.0
      - compute_particles            - Compute-shader particle system
      - occlusion_culling            - Hi-Z GPU occlusion culling
      - opencl_matmul                - OpenCL matrix multiply (built only when OpenCL is found)

    Each sample is installed into `bin/<sample>/` with its shaders/assets staged
    alongside the binary: per-sample shaders in `Supportingfiles/`, the shared
    font shaders in `FontShaders/`, and any extra data (`models/`, `assets/`,
    `cl_kernels/`). The samples load these with paths relative to the working
    directory, so run each one from its own install directory (e.g.
    `cd bin/trianglesample && ./trianglesample`).

    The windowing backend of the framework-based samples is selected at configure
    time with `-D SDK_Platform=<wayland|x11|fbdev>` (default `wayland`), matching
    the legacy GPU_SDK_OpenGLES `ENABLE_WAYLAND` / `ENABLE_X11` / `ENABLE_FBDEV`
    options:
      - `wayland` - render through a Wayland (xdg-shell) surface; run under a
        compositor such as Weston.
      - `x11`     - render into an X11 window; run under an X server.
      - `fbdev`   - render directly to the Linux framebuffer (EGL_DEFAULT_DISPLAY);
        run from a bare console.
    Exactly one backend is compiled, and only that backend's system libraries are
    linked. `eglrenderingsample` and `egltexturevideosample` are standalone legacy
    X11/fbdev EGLImage demos (they manage their own surface) and always build
    against X11 independently of `SDK_Platform`.
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
    - adas-lane-detection     - Lane visualization via Sobel edge detection (ADAS)
    - ml-classifier           - Average-intensity feature + linear "Bright/Dark" classifier

    Most OpenCL samples read their inputs from `assets/` (relative to the
    executable) and write results (e.g. `output.bmp`/`output.png`) into the
    current working directory.


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
source /yocto/build/tmp/deploy/sdk/environment-setup-cortexa72-cortexa53-amd-linux

XLNX_SDK=/path/to/yocto_sdk \
cmake \
    -G Ninja \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_TOOLCHAIN_FILE=xlnx.cmake \
    -D SDK_Platform=wayland \
```

`SDK_Platform` selects the OpenGL ES samples' windowing backend
(`wayland` (default) | `x11` | `fbdev`); see the `samples/opengles` notes above.

## Compile

```bash
ninja
```


# Running the OpenCL samples

Each OpenCL sample is installed into `bin/<sample>/` together with its `assets/`
directory (kernels and input images). Because the samples load their kernels and
inputs with paths relative to the working directory (e.g. `assets/foo.cl`,
`assets/input.bmp`), run each sample from inside its own directory:

```bash
cd bin/gaussian_filter
./gaussian_filter
```

Notes:

- Outputs are written to the current directory: filter/image samples produce
  `output.bmp` or `output.png` (the Sobel samples additionally write
  `output-dX.bmp` and `output-dY.bmp`).
- Samples print OpenCL kernel profiling information (queued/wait/run time) on
  completion.
- A `[XRT] ERROR: No devices found` line may be printed by the XRT OpenCL ICD
  while it probes for FPGA/Alveo devices. It is harmless; the Mali GPU device is
  selected through its own ICD and the kernels still run.

Sample-specific inputs and arguments:

- `matrix-multiply` accepts command line options (run `./matrix_multiply -h` for
  the full list, including `-va` to validate against a CPU reference and
  `-csv <file>` to dump results).
- `mandelbrot-benchmark` and `adas-lane-detection` expect a 1024x1024
  `assets/input.bmp`.
- `depth-map` expects a stereo pair `assets/left.bmp` and `assets/right.bmp`.
- `sobel-filter` expects an `assets/input.bmp` whose width is a multiple of 16.
- `gaussian-filter` reads `assets/input.png`; all other image samples read
  `assets/input.bmp`.
- `integer-64-bit` requires the `cl_khr_int64_base_atomics` device extension.
- `hello-world-c`, `hello-world-opencl`, `vector-hello-world`, `opencl-template`
  and `sgemm` take no inputs and produce no image output.


---

# 🎨 Complete Sample Reference

> **Note on images:** The snapshots below reference the artwork from the
> original per-API SDK repositories (`doc_imgs/` for OpenGL ES, `doxygen/images/`
> for OpenCL and Vulkan). Drop the corresponding image files into those folders
> to have them render here.

## OpenGL ES Samples

### **OpenGL ES 2.0 Samples**

#### 1. **Triangle Sample**
![Triangle Sample](doc_imgs/TriangleSample.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/trianglesample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** A basic triangle rendered in OpenGL ES 2.0 with color
interpolation across vertices and rotation matrices for dynamic animation. A
perfect starting point for learning OpenGL ES fundamentals.

**Key Techniques:** Vertex shaders, color interpolation, basic rendering pipeline, matrix transformations

---

#### 2. **MultiCube Sample**
![MultiCube Sample](doc_imgs/cube.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/multicubesample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates basic cube rendering with color interpolation and
rotation matrices, and the rendering of multiple objects. Essential for
understanding 3D transformations.

**Key Techniques:** 3D graphics, matrix operations, depth testing, multiple object rendering

---

#### 3. **Texture MipMap Sample**
![MipMap Sample](doc_imgs/mipmap.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/texturemipmapsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Shows the Mali GPU decompressing a compressed texture on the fly
with multiple MIPMAP levels used as textures (stored as compressed ETC files).

**Key Techniques:** ETC1 compression, mipmap generation, texture optimization, GPU decompression

---

#### 4. **Alpha Depth Sample**
![Alpha Sample](doc_imgs/alpha.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/alpha_depthsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates transparency rendering and depth projection effects
with proper alpha blending and depth testing.

**Key Techniques:** Alpha blending, depth projection, transparency effects, render order management

---

#### 5. **Frame Buffer Object (FBO) Sample**
![FBO Sample](doc_imgs/FBO.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/framebufferobject-fbosample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Render-to-texture using a framebuffer object: a colored cube is
rendered to a texture and applied across the faces of an outer cube, with both
cubes animated by rotation matrices.

**Key Techniques:** Render-to-texture, FBO management, multi-pass rendering, texture as render target

---

#### 6. **Particle Rendering Sample**
![Particle Sample](doc_imgs/particle.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/particlerenderingsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Particle effect simulation on the Mali GPU. Draws many particles
created from a small texture, each with its own lifetime and color, animated via
matrices.

**Key Techniques:** Point sprites, physics simulation, dynamic particle management, GPU-based effects

---

#### 7. **Shadow Mapping Sample**
![Shadow Sample](doc_imgs/ShadowSample.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/shadowsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Showcases lighting and shadow effects on the Mali 400 GPU with
real-time shadow rendering techniques.

**Key Techniques:** Shadow maps, depth textures, multi-pass rendering, advanced lighting

---

#### 8. **Waveform Pattern Sample**
![Wave Sample](doc_imgs/wave.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/waveformsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Waveform rendering using only GLSL shader code while minimizing
vertices; multiple waveforms are implemented with math functions in the pixel
shader.

**Key Techniques:** Vertex displacement, procedural animation, mathematical functions in shaders

---

#### 9. **RotoZoom Sample**
![RotoZoom Sample](doc_imgs/roto.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/rotozoom/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Classic demo effect showcasing 2D transformations and texture
sampling with rotation and zoom.

**Key Techniques:** 2D transformations, texture sampling, classic demo effects

---

#### 10. **Mesh Rendering Sample**
![Mesh Sample](doc_imgs/mesh.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/meshrendersample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates 3D object/mesh rendering for `.obj` files: loading
and rendering complex 3D geometry.

**Key Techniques:** 3D model loading, complex geometry, material systems, .obj file parsing

**Note:** *USER MUST PROVIDE THEIR OWN OBJECT FILE*

---

#### 11. **YUV2RGB Conversion Sample**
![YUV2RGB Sample](doc_imgs/Yuv2rgb.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/yuv2rgbsample/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11

**Description:** Fast color conversion from YUV to RGB in a GLSL shader on the
fly, rendering the converted image to the framebuffer frame by frame.

**Key Techniques:** Color space conversion, video textures, GPU-based color conversion

**Note:** *USER MUST PROVIDE THEIR OWN YUV IMAGE FILE*

---

#### 12. **EGL Image Texture Swapping Sample**
![EGL Sample](doc_imgs/EGL1.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/eglrenderingsample/`

**Supported Window Environments:** ✅ FBDEV (standalone X11/fbdev)

**Description:** Showcases the EGL Image extension, which defines an EGL resource
type (EGLImage) suitable for sharing 2D arrays of image data between client APIs.

**Key Techniques:** EGL Image extension, texture sharing, EGL resource management

---

#### 13. **EGL Image Live Video Streaming Sample**
![EGL Video Sample](doc_imgs/Video_Texture.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/egltexturevideosample/`

**Supported Window Environments:** ✅ FBDEV (standalone X11/fbdev)

**Description:** Uses the EGL Image extension in real time: takes a live feed
from a USB camera (`/dev/video0`) and streams the data as an OpenGL ES texture,
updating it each frame. Using EGL Image instead of `glTexImage2D` is much faster.

**Key Techniques:** EGL Image extension, real-time video streaming, live camera feed, texture streaming

---

#### 14. **EGL Preserve Sample**
![EGL Preserve Sample](doc_imgs/EGL_preserve.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/eglpreserve/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates EGL buffer preservation (`EGL_BUFFER_PRESERVED`)
and context management for efficient rendering and resource management.

**Key Techniques:** EGL context preservation, buffer management, efficient rendering

---

#### 15. **BBOX Sample**
![BBOX Sample](doc_imgs/bbox.png)

**📁 Source**: `samples/opengles/opengl_es_2.0/bbox/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Demonstrates bounding-box rendering, reading the bbox
coordinates and class label from a text file in the same directory.

**Key Techniques:** BBOX rendering, on-screen text

---

### **OpenGL ES 3.0 Samples**

#### 1. **Compute Particles Sample**
![Compute Particles Sample](doc_imgs/compute_particles.png)

**📁 Source**: `samples/opengles/opengl_es_3.0/compute_particles/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** GPU compute-shader particle system using modern OpenGL ES 3.x
compute capabilities for high-performance particle simulation.

**Key Techniques:** GPU compute shaders, SSBO, compute dispatch, memory barriers, parallel processing

---

#### 2. **Occlusion Culling Sample**
![Occlusion Culling Sample](doc_imgs/occlusion_culling.png)

**📁 Source**: `samples/opengles/opengl_es_3.0/occlusion_culling/`

**Supported Window Environments:** ✅ FBDEV  ✅ X11  ✅ Wayland

**Description:** Visibility optimization using Hi-Z GPU occlusion culling for
improved rendering performance in complex scenes.

**Key Techniques:** Hi-Z occlusion, indirect rendering, visibility optimization

---

#### 3. **OpenCL Matrix Multiplication Sample**

**📁 Source**: `samples/opengles/opengl_es_3.0/opencl_matmul/`

**Description:** Demonstrates GPGPU computing using OpenCL for high-performance
matrix operations with OpenGL ES interoperability (built only when OpenCL is
found).

**Key Techniques:** GPGPU computing, OpenCL interop, matrix operations, parallel computing

---

## OpenCL Samples

#### 1. **Hello World OpenCL**
![Hello World Sample](doxygen/images/hello_world_output.png)

**📁 Source**: `samples/opencl/hello-world-opencl/`

**Description:** Basic integer array addition implemented in an OpenCL kernel.

**Key Techniques:** OpenCL initialization, kernel creation, buffer management, basic parallel computation

---

#### 2. **Hello World C**

**📁 Source**: `samples/opencl/hello-world-c/`

**Description:** Simplified C-style OpenCL sample demonstrating basic concepts
without C++ abstractions. No output — if it runs successfully, your machine can
run OpenCL.

**Key Techniques:** C-style OpenCL API usage, context creation, simple kernel execution

---

#### 3. **Mandelbrot Set**
![Mandelbrot Sample](doxygen/images/Mandelbrot.png)

**📁 Source**: `samples/opencl/mandelbrot/`

**Description:** Generates Mandelbrot fractal data, testing each pixel to
determine the iterations needed to rule out set membership, output as a grayscale
bitmap (`output.bmp`).

**Key Techniques:** Parallel fractal computation, 2D indexing, mathematical computation on GPU

---

#### 4. **Mandelbrot Benchmark**
![Mandelbrot Benchmark Sample](doxygen/images/Mandelbrot_benchmark.png)

**📁 Source**: `samples/opencl/mandelbrot-benchmark/`

**Description:** Performance-focused Mandelbrot generator with timing and image
perturbation for benchmarking GPU compute. Expects a 1024x1024 `assets/input.bmp`.

**Key Techniques:** Performance measurement, GPU benchmarking, optimized parallel computation

---

#### 5. **Image Scaling**
![Image Scaling Sample](doxygen/images/ImageScaling.png)

**📁 Source**: `samples/opencl/image-scaling/`

**Description:** Uses OpenCL image objects and a sampler to resize images on the
GPU.

**Key Techniques:** OpenCL image objects, 2D image processing, bilinear interpolation, image resampling

---

#### 6. **Sobel Filter**
![Sobel Filter Sample](doxygen/images/SobelFilter.png)

**📁 Source**: `samples/opencl/sobel-filter/`

**Description:** Edge detection using the Sobel operator with vector loads,
computing X and Y gradients and combining them. Expects an `assets/input.bmp`
whose width is a multiple of 16.

**Key Techniques:** Edge detection, convolution kernels, gradient computation, vector loads

---

#### 7. **Sobel Filter (No Vectors)**
![Sobel No Vectors Sample](doxygen/images/SobelFilter.png)

**📁 Source**: `samples/opencl/sobel-filter-no-vectors/`

**Description:** Sobel edge detection without vector types — one pixel per work
item — demonstrating an alternative optimization approach.

**Key Techniques:** Scalar-based image processing, alternative optimization strategies, edge detection

---

#### 8. **Gaussian Filter**
![Gaussian Filter Sample](doxygen/images/GaussianFilter.png)

**📁 Source**: `samples/opencl/gaussian-filter/`

**Description:** Applies a Gaussian blur to a PNG image (`assets/input.png`) for
noise reduction and smoothing.

**Key Techniques:** Gaussian blur, separable convolution, image smoothing, noise reduction

---

#### 9. **Convolution Filter**
![Convolution Filter Sample](doxygen/images/ConvolutionFilter.png)

**📁 Source**: `samples/opencl/convolution-filter/`

**Description:** General-purpose 3x3/5x5 convolution filtering framework
supporting various kernel types.

**Key Techniques:** General convolution, custom kernels, flexible image processing framework

---

#### 10. **FIR Filter**
![FIR Filter Sample](doxygen/images/FirFilter.png)

**📁 Source**: `samples/opencl/fir-filter/`

**Description:** Finite Impulse Response filtering for signal and image
processing on the GPU.

**Key Techniques:** FIR filtering, signal processing, digital filter implementation

---

#### 11. **Depth Map**
![Depth Map Sample](doxygen/images/DepthMap.png)

**📁 Source**: `samples/opencl/depth-map/`

**Description:** Stereo-vision depth map generation for obstacle detection.
Expects a stereo pair `assets/left.bmp` and `assets/right.bmp`.

**Key Techniques:** Stereo vision, depth estimation, correlation algorithms, 3D reconstruction

---

#### 12. **ML Classifier**
![ML Classifier Sample](doxygen/images/MLClassifier.png)

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

**Description:** Pixel sum / sum-of-squares using 64-bit integers and atomics.
Requires the `cl_khr_int64_base_atomics` device extension.

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

**📁 Source**: `samples/opencl/adas-lane-detection/`

**Description:** Lane visualization via Sobel edge detection (ADAS use case).
Expects a 1024x1024 `assets/input.bmp`.

**Key Techniques:** Edge detection, ADAS, image processing

---

## Vulkan Samples

#### 1. **Triangle**
![Hello Triangle Sample](doxygen/images/hello_triangle.png)

**📁 Source**: `samples/vulkan/triangle/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Minimal Vulkan sample that renders a single triangle.
Demonstrates swapchain setup, render passes, and the basic graphics pipeline.

**Key Techniques:** Vulkan initialization, swapchain, render pass, graphics pipeline, vertex buffers

---

#### 2. **Texture (Rotating)**
![Rotating Texture Sample](doxygen/images/rotating_texture.gif)

**📁 Source**: `samples/vulkan/texture/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Renders a textured quad and animates its rotation. Demonstrates
texture loading, samplers, and uniform buffer updates.

**Key Techniques:** Texture loading, samplers, uniform buffers, animation, transformation matrices

---

#### 3. **Basic Compute**
![Compute Sample](doxygen/images/basic_compute.gif)

**📁 Source**: `samples/vulkan/basic-compute/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Compute-shader particle simulation rendered via graphics,
demonstrating compute-to-graphics synchronization and storage buffers.

**Key Techniques:** Compute shaders, storage buffers, memory barriers, dispatch, particle simulation

---

#### 4. **Multithreading**
![Multithreading Sample](doxygen/images/multithreading.gif)

**📁 Source**: `samples/vulkan/multithreading/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Multi-threaded secondary command buffer recording and submission
for improved CPU-side performance.

**Key Techniques:** Multithreaded command buffers, synchronization, parallel rendering

---

#### 5. **Multisampling**
![Multisampling Sample](doxygen/images/multisampling.gif)

**📁 Source**: `samples/vulkan/multisampling/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Enables and uses multisample anti-aliasing (MSAA) for smoother
rendering.

**Key Techniques:** Multisample anti-aliasing, render pass setup, resolve attachments

---

#### 6. **Spinning Cube**
![Spinning Cube Sample](doxygen/images/spinning_cube.gif)

**📁 Source**: `samples/vulkan/spinning-cube/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Renders a textured 3D cube with rotation animation and depth
buffering.

**Key Techniques:** 3D transformations, depth buffer, animation, uniform buffers

---

#### 7. **Multi-pass Rendering**
![Multi-pass Sample](doxygen/images/multipass.gif)

**📁 Source**: `samples/vulkan/multipass/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Deferred shading using multiple subpasses and framebuffers,
useful for post-processing and advanced rendering effects.

**Key Techniques:** Render passes, subpasses, framebuffers, image attachments, post-processing

---

#### 8. **ASTC Texture Compression**
![ASTC Sample](doxygen/images/astc.gif)

**📁 Source**: `samples/vulkan/astc/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Loads and displays ASTC compressed textures, with fallback to
PNG if ASTC is not supported.

**Key Techniques:** ASTC texture loading, compressed textures, format fallback

---

#### 9. **Mipmapping**
![Mipmapping Sample](doxygen/images/mipmapping_manually_generated.gif)

**📁 Source**: `samples/vulkan/mipmapping/`

**Supported Platforms:** ✅ Wayland  ✅ X11 (Display/DRM-KMS)

**Description:** Shows the use and visual effect of mipmaps, demonstrating both
pre-generated and GPU-generated mipmaps.

**Key Techniques:** Mipmaps, texture sampling, image blitting, LOD selection

---

# 🏗️ Framework Architecture

The unified SDK provides shared support libraries under `lib/sdk/`, reused by all
sample sets:

- **common** — Graphics-API-agnostic helpers (math, timing, image I/O, file
  utilities).
- **egl** — EGL context/surface management for the OpenGL ES samples.
- **gles** — OpenGL ES helper classes and the `SdkGlesFramework` windowing
  abstraction (Wayland / X11 / fbdev), font rendering, texture and matrix
  utilities ported from the legacy GPU_SDK_OpenGLES CoreFramework.
- **vulkan** — RAII-style Vulkan helper classes (context, swapchain, command
  buffers, assets) backing the Vulkan samples.
- **vulkan-strings / vulkan-vendor-ids** — Debugging helpers that resolve Vulkan
  enums and GPU vendor names to human-readable strings.

The OpenCL samples build on the shared `common` image/bitmap helpers plus a thin
OpenCL context/kernel management layer.

# 🔧 Adding New Samples

The build system is designed to be general. To add a new sample, copy an existing
one in the relevant `samples/<api>/` directory, rename the target in its
`CMakeLists.txt`, and add an `add_subdirectory(<newsample>)` to the parent
`CMakeLists.txt`. Place source files in the sample folder, shaders/kernels and
data under the sample's `Supportingfiles/`, `shaders/`, `assets/`, or
`cl_kernels/` directory as appropriate.

# 📄 License

This project is licensed under the terms in the [LICENSE](LICENSE) file.

# 🏆 Acknowledgments

- AECG PSAS — System Software
- Mali GPU driver developers
- The Khronos OpenGL ES, OpenCL, and Vulkan communities

## 🏷️ Trademarks

OpenCL and Vulkan are registered trademarks of the Khronos Group Inc.
