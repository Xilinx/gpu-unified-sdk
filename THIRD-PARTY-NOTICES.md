# Third-Party Notices

The gpu-unified-sdk includes, or is derived from, third-party software. AMD's own
contributions are released under the BSD-3-Clause license in the root `LICENSE`
file. The third-party components below retain their original licenses; where AMD
has modified third-party files, a `Modifications Copyright (C) 2024-2026 Advanced
Micro Devices, Inc.` line has been added beneath the original notice and the
original copyright and permission notices have been preserved in full.

This file lists every non-AMD component distributed in this repository.

---

## 1. ARM Vulkan SDK  (MIT)

**Used by:** the Vulkan framework (`lib/sdk/Vulkan/framework/`) and the Vulkan
samples (`samples/vulkan/*`). These files are derived from ARM's Vulkan SDK
(originally `namespace MaliSDK`, renamed to `VersalGPUSDK`). The original ARM
per-file MIT notice has been restored on each derived file.

**Origin:** ARM `vulkan-sdk` (github.com/ARM-software/vulkan-sdk)
**License:** MIT

```
Copyright (c) 2016-2017, ARM Limited and Contributors

SPDX-License-Identifier: MIT

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

---

## 2. ARM Mali OpenGL ES SDK for Android  (MIT)

**Used by:** the OpenGL ES samples `compute-particles`, `occlusion-culling`,
`rotozoom`, and `eglpreserve` under `samples/opengles/` (source and GLSL shaders).
These are derived from ARM's Mali OpenGL ES SDK for Android (each is an ARM
`advanced_samples` entry, verified against ARM's published upstream). Per-file
years follow upstream (`rotozoom` and `eglpreserve` = 2012-2017;
`compute-particles` and `occlusion-culling` = 2014-2017).

**Origin:** ARM `opengl-es-sdk-for-android` (github.com/ARM-software/opengl-es-sdk-for-android)
**License:** MIT — `Copyright (c) 2012-2017, ARM Limited and Contributors`
(same MIT text as Section 1, with the year range per file).

---

## 3. ARM Mali font engine  (MIT) — headered

**Used by:** `lib/sdk/GlesFramework/FontEngine.*` and its shared font shaders
(`font.frag`/`font.vert`, reused byte-identically by `bbox` and `eglpreserve`).
The ARM MIT notice + AMD modifications line has been applied to all **8** of these
files. The notice text is ARM's standard MIT (identical to Sections 1-2); the year
`2014-2017` is matched to the Android ARM SDK group (Section 2).

**Origin:** ARM Mali OpenGL ES SDK (shared font engine)
**License:** MIT — `Copyright (c) 2014-2017, ARM Limited and Contributors`

> **Note (2026-07-02):** the `shadowsample` sample
> (`samples/opengles/opengl_es_2.0/shadowsample/*`) was **previously listed here**
> as ARM-derived. On review this was **incorrect**: the sample's `M`-prefixed class
> family (`MRendererProgram`, `MDRRenderer`, `MGeometryTorus`, `ShadowMap`, etc.)
> is **AMD-authored** (`namespace SDKXilinx`; upstream headers were
> `Copyright (c) _2024, Advanced Micro Devices`), and no such framework exists in
> any ARM Mali SDK (ARM's shadow sample uses `namespace MaliSDK` with plain
> `Matrix`/`Text`/`Shader` classes). The 13 files that had been mis-tagged
> MIT/ARM (`ShadowMap.cpp/.h`, `3d.h`, `MRendererProgram.cpp`, `MVector4.h`, and
> the 8 `ShadowMap_*` GLSL shaders) have been **re-attributed to BSD-3-Clause /
> AMD**, so the entire `shadowsample` folder is now uniformly AMD BSD-3-Clause and
> is **not** third-party.

---

## 4. Linaro glmark2 — RESOLVED BY REMOVAL of `meshrendersample`

The `samples/opengles/opengl_es_2.0/meshrendersample` sample was **removed in its
entirety** (2026-06-26). It contained glmark2-derived code under two different
licenses:

- **MIT `libmatrix`** (`mat.*`, `vec.h`, `stack.h`, `util.*`, `namespace
  LibMatrix`) — derived from glmark2's MIT `src/libmatrix`; these were
  redistributable but are no longer present.
- **GPL-3.0 glmark2 core** (`mesh.{cpp,h}`, `3DModel.{cpp,h}`) — derived from
  glmark2's **core** `src/mesh.{cpp,h}` / `src/model.{cpp,h}` (`Copyright © 2008
  Ben Smith`, `© 2010-2011 Linaro Limited`). Measured derivation: `mesh.h` 90%
  (near-verbatim `Mesh` class), `mesh.cpp` 81%, `3DModel.*` confirmed by
  identical `Model::obj_get_face`/`convert_to_mesh`/`append_object_to_mesh`
  family. GPL-3.0 is strong copyleft and **incompatible** with this BSD-3
  release; a header cannot cure it.

Because the GPL-3.0 files could not ship under BSD-3 and the rest of the sample
depended on them, the whole sample was deleted rather than partially remediated.
This **removes the glmark2 dependency (both MIT and GPL) entirely** — no glmark2
code remains in the tree. The sample was also de-registered from
`samples/opengles/opengl_es_2.0/CMakeLists.txt` and the README.

---

## 5. ARM Mali OpenCL SDK  (license TBD) — provenance confirmed, exact header pending

**Used by:** the OpenCL samples (`samples/opencl/*/main.cpp` and `.cl` kernels).
These derive from ARM's Mali OpenCL SDK. The exact upstream license and per-file
notice will be confirmed and applied once that upstream is available.

**Origin:** ARM Mali OpenCL SDK
**License:** to be confirmed from upstream (MIT expected, consistent with ARM's
other Mali SDKs — must be verified before release).

---

## 6. stb_image  (Public Domain / MIT)

**Used by:** `3rdparty/stb/stb_image.h` (image loading), bundled verbatim.

**Origin:** stb (github.com/nothings/stb), `stb_image.h` **v2.13**, by Sean Barrett.
**License:** dual-licensed — Public Domain (Unlicense) **or** MIT. The full
license text is embedded at the end of `3rdparty/stb/stb_image.h`.

---

## 7. Quaternius "Pickup Truck" 3D model  (CC0 1.0) — REMOVED

This CC0 model (`car.obj` / `car_texture.png`) was bundled **only** inside the
now-removed `meshrendersample/models/` directory. With that sample deleted, the
model is no longer present in the tree and is listed here for historical
completeness only. (CC0 carries no obligations regardless.)

---

## Notes for the SCL submission

- The repository's **"Non-AMD Content"** field should be set to **Yes**, listing
  the components above.
- Sections 1, 2 and 3 (ARM Vulkan SDK, ARM Mali OpenGL ES SDK for Android, and
  the ARM Mali font engine) are fully remediated in-tree: the ARM MIT notice plus
  the AMD modifications line is present on every derived file — **142 files** total
  (Vulkan 56; GLES Android samples `compute-particles`/`occlusion-culling`/
  `rotozoom`/`eglpreserve` 80; ARM font engine + shared font shaders 6).
- **Correction (2026-07-02):** the `shadowsample` sample was previously counted
  here as 21 ARM Mali-Linux files. It is **not** ARM-derived (AMD-authored
  `M`-prefixed / `SDKXilinx` framework); its 13 mis-tagged files were re-attributed
  to **BSD-3-Clause / AMD**, and it is no longer third-party. See Section 3.
- **Section 4 (glmark2): RESOLVED by deleting the entire `meshrendersample`.**
  That sample contained GPL-3.0 glmark2-core code (`mesh.*`, `3DModel.*`) that
  could not ship under BSD-3, plus MIT libmatrix files. The whole sample was
  removed, so **no glmark2 code (MIT or GPL) remains** and there is no GPL
  blocker left from this group. The sample was de-registered from its parent
  `CMakeLists.txt` and the README.
- Section 7 (Quaternius CC0 car model) was bundled only inside that sample and is
  therefore also gone.
- Section 5 (ARM Mali OpenCL SDK) is confirmed by code analysis but its files are
  still header-less; the ARM notice and Legal's MIT-vs-EULA confirmation are the
  main remaining provenance gate before release.
- All ARM material identified so far is the **open-source MIT** release, not a
  proprietary Mali SDK EULA. Legal should confirm the same for the OpenCL SDK
  (Section 5) before release.
- The Xilinx brand logo demo texture (`texturemipmapsample/Supportingfiles/
  xilinx.png` + 12 `xilinx_mip_*.pkm`) was **removed** — it was unused (the sample
  loads the `KR_Front` texture set), so no Xilinx brand asset ships.
