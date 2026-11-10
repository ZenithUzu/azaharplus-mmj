# MMJ integration with OpenGL and Vulkan

The graphics hacks use one resolved per-title profile (`MmjGpuSettings`). The profile
is derived from the original title lists and the effective settings, then applied as
a complete unit to either hardware rasterizer and its texture cache. Individual
backend calls no longer have to remember extra flags for each texture operation.

## Execution paths

| Hack | Shared implementation | OpenGL connection | Vulkan connection |
|---|---|---|---|
| Accurate multiplication | Original Azahar override plus gated MMJ override in `ResolveMmjGpuSettings` | Profile passed to shader selection and cache loading | Pipeline shader profile updated immediately on application, as well as during cache loading |
| Skip slow draws | PICA draw dispatch, after hardware acceleration fails | Same PICA path | Same PICA path |
| Skip texture copies | `RasterizerCache::AccelerateTextureCopy` | Cache receives full profile | Cache receives full profile |
| Disable extra clipping plane | Shared draw uniforms and generated vertex/geometry shaders | GL clip-distance enable state uses the same policy as uniforms | Generated clipping distances use the shared uniforms when supported by the GPU |
| Linear magnification | Shared sampler parameters and sampler cache | All configured texture/cube sampler requests use cache state | All configured texture/cube sampler requests use cache state |
| Skip tiny CPU writes | Shared invalidation path, CPU writes of 1–8 bytes only | Shared cache policy | Shared cache policy |
| Display-transfer remap | Shared accelerated transfer, four original address/height guards | Shared cache policy | Shared cache policy |
| Disable shadow draws | Shared shadow predicate, both accelerated and software-vertex batches | Early draw suppression, pending batch cleared | Early draw suppression, pending batch cleared |

CPU-usage limits, FMV ticks and Y2R conversion skipping run in the shared CPU/timing/HLE
code, independently of the graphics API. Duplicating them inside each renderer would
apply them at the wrong lifecycle stage. Their existing per-game switches still apply
with either OpenGL or Vulkan.

## Corrected lifecycle and coherency problems

- Applying a profile submits any queued software-vertex batch **before** changing its
  state, then notifies the backend once with the complete new profile. Switching title
  IDs also performs this transition when the flags happen to be identical. Reapplying
  the same profile for the same title does not add another draw.
- Both backends transfer the full profile to their cache. Texture copies and sampler
  creation read that state directly, instead of receiving individual optional flags
  at every call site.
- Vulkan's accurate-multiplication shader profile is synchronized immediately. It no
  longer depends solely on loading a disk cache to receive the current value.
- Recreating a renderer initializes its shader resources even when **disk** caching
  is disabled. Vulkan still requires its in-memory shader cache in that case; each
  backend already handles the disk-persistence preference internally.
- Both backend uniform streams are marked dirty when the profile changes. A viewport
  check no longer clears an already pending vertex-uniform update.
- OpenGL's extra clip-distance enable flag and the shared vertex uniform now use
  exactly the same per-title clipping predicate. The fixed PICA clipping plane stays
  enabled; this change does not remove normal clipping globally.
- An intentionally ignored tiny CPU write returns before changing either surface
  validity **or GPU dirty-region ownership**. Previously the surface could be kept
  while the dirty-region map was still erased. GPU-owned copy invalidations are never
  ignored by this heuristic.
- The dedicated software renderer does not inherit hardware slow-draw suppression,
  which otherwise could suppress all geometry for a listed title.

The texture-copy shortcut still only handles a missing cached source. Other validation
failures retain their normal fallback. The address-remap hack remains on the accelerated
display-transfer path, as in MMJ; this does not invent a new CPU-blitter remap.

## Verification and limits

The standalone test target now exercises the real profile resolver with every original
graphics title list, independent switch gating, upstream accuracy preservation,
unlisted titles, full profile propagation and title transitions, renderer recreation
at the common interface, clipping/shadow decisions, CPU-versus-GPU invalidation policy,
and the four exact address remaps. It does not mock a graphics driver or claim to render
frames. Compile validation includes the actual OpenGL and Vulkan translation units.

Run:

```sh
cmake -S src/tests/mmj -B build-mmj -DCMAKE_BUILD_TYPE=Release
cmake --build build-mmj --parallel 2
ctest --test-dir build-mmj --output-on-failure
python3 tools/verify-mmj-port.py
```

Visual correctness and performance still require game runs on each backend, including
cache on/off, a renderer restart, and per-game switches on/off. Experimental hacks
remain off by default. The installed legacy Android app has a different signing key
from the new stable builds, so no phone installation or migration is performed as part
of these source changes. See `INTEGRATION_2026_11.md` for signing and runtime status.
