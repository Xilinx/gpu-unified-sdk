
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
