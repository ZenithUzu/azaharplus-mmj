# November 2026 integration audit

This is an incremental integration, not a claim that the entire Citra MMJ emulator has
been transplanted or that every game works. Compilation, regression tests, visual
correctness, and on-device performance are separate checks.

## Source revisions inspected

- Fork's maintained branch: `azaharplus-mmj`, starting at `6dfc72ca3`.
- AzaharPlus `AzaharPlus`: `cf9068844`. Already an ancestor of the starting branch;
  there were no newer commits on that upstream branch to import when fetched.
- Azahar `master`: `6b4e7f2e6` (2026-11-06).
- Citra MMJ `master`: `302b0c3bc4312bc96489c11584a97664894b343f`.

Each imported upstream change retains its original author and a `cherry picked from`
commit reference. This branch intentionally does not merge all of Azahar `master`:
its Android settings/input rewrites and other large features need their own adaptation
to AzaharPlus and the fork's UI. A direct merge produces extensive conflicts because
parts of AzaharPlus use a different commit history for upstream changes.

## Imported Azahar changes

28 upstream commits were imported in this batch:

- Shader JIT masked-write optimizations for AArch64 and x86-64 (#2597).
- Input/audio latency improvements (#2700, #2706), including Android performance
  hints and limiting GPU work by guest frames (#2755). New configurations default
  to audio stretching off; existing explicit settings are preserved.
- Vulkan min/max color blending (#2427), procedural texture LUT dirtiness (#2627),
  mip-zero image-view aliasing (#2482), stride-alias validation (#2451), and stale
  asynchronous custom texture upload rejection (#2688).
- Geometry-aware command-list suppression (#2467); Anime4K restricted to OpenGL.
- Android shortcut lifecycle fixes (#2656, #2658), turbo reset (#2657), hidden file
  scanning (#2362), secondary screen wake lock (#2773), touch coordinate fix (#2766),
  and expanded custom-driver capability checks (#2767).
- Thumb ADD-PC interpreter correction (#2710), CPU time-limit validation (#2711),
  wireless reboot information (#2712), deterministic filesystem timing (#2732),
  BOSS header endianness (#2740), NWM undefined behavior (#2770), and SDL lock order
  (#2769).
- DSP sleep completion on the next audio frame, file append position (#2707), and
  modern zstd CMake target compatibility.

This does not include MSAA, the Android input/settings overhauls, ROM bundle support,
CSND support, the new battery UI, free camera, or every desktop feature from upstream.

## MMJ integration corrections

- All 35 `cpu_usage_limit` title IDs now have their original per-core profiles:
  `(1,4,2,2)`, `(2,4,2,2)`, or `(1,1,1,1)`. The last profile adds the three regional
  Binding of Isaac: Rebirth IDs and Futuridium EP Deluxe. The existing experimental
  switch controls all three profiles and remains off by default.
- CPU profile application explicitly resets all shifts for an unlisted title or a
  disabled switch. Logs also report the inactive `(0,0,0,0)` profile.
- Clip-plane uniforms respond to a changed MMJ switch even when PICA registers have
  not been written again. The fixed clipping plane remains unchanged.
- Android MMJ per-game settings previously had no complete save/load path. The game
  details sheet now opens the MMJ settings screen for that title. Changes are saved
  to `GameSettings/<16-digit uppercase title ID>.ini` and loaded before core startup,
  so CPU, FMV, GPU and Y2R hacks see the same overrides.
- Only changed MMJ switches are written; unrelated INI contents are preserved.
  Setting a switch to its global value removes its override and restores inheritance.
  Missing game settings files are handled normally. Shared Kotlin enum values are
  reset before loading another screen so one game's MMJ settings cannot leak into
  another game's UI. Other per-game sections are outside this implementation.

## Reproduce the checks

```sh
git submodule update --init externals/catch2
cmake -S src/tests/mmj -B build-mmj -DCMAKE_BUILD_TYPE=Release
cmake --build build-mmj --parallel 2
ctest --test-dir build-mmj --output-on-failure

git fetch --no-tags --depth=1 https://github.com/weihuoya/citra.git 302b0c3bc4312bc96489c11584a97664894b343f
python3 tools/verify-mmj-port.py

cd src/android
./gradlew assembleVanillaRelease -Pabi=arm64-v8a -PappIdSuffix=.mmj
```

The native tests exercise the real `HackManager` and compiled title tables: every CPU
profile, disabled/unlisted behavior, duplicate IDs, table counts, and separation from
upstream accurate multiplication. The Python verifier compares the ported lists with
MMJ's pinned `config-games.ini`; it records the existing Tales of the Abyss slow-draw
exclusion and the two lists originally taken from `LoadOverrides`.

The `MMJ integration validation` workflow performs source-list verification, native
regression tests and a full Android arm64 build on PRs targeting `azaharplus-mmj`.
Its APK is a test artifact; it does not publish or delete releases.

## Runtime verification still required

A device was subsequently connected and the owner identified Pokemon Ultra Sun as
the first test title. However, the installed APK is byte-for-byte the original
`build1` (SHA-256 `518a4075c3b8d735e576c821ac0454b9e076c2e77ad26449fa6ab02f1e016160`).
Its signing certificate SHA-256 is
`32bf0bafed6c6006adf302bbe090208778cebeff49e033fe2623c11231beeb3c`.
The available local debug keystore has a different certificate. The release notes
confirm that `build1` used a temporary CI key. No matching original private signing
key has been located, so no installation, uninstallation, replacement package or
device-data modification was performed.

The original Actions job logs were subsequently inspected: the signing secrets were
empty and it explicitly reported a throw-away debug key. A new APK from Actions run
`42712610494` was downloaded and verified; its certificate starts with `12c7acae` and
does not match `build1` either. The generic runner signing path therefore does not
preserve that old identity.

Following the owner's acceptance of development signing, a dedicated **fixed public
development key** is bundled in `src/android/keystore`. Its certificate SHA-256 is
`cf14c1c48473065a2c29049709104e3398828685ce9662084e5d1856879737f4`.
Release builds on Actions and locally now select that same key by default. Both PR
validation and publication verify the resulting certificate. A private signing key
can still be supplied explicitly, with its expected certificate pinned separately.

This fixes signing consistency for future builds, but starts a new baseline: it
cannot update the existing `build1` installation in place. No migration, installation,
uninstallation or app-data change is performed automatically. No new FPS gains or
game compatibility results are asserted.
The experimental CPU timing, FMV timing, CPU-write invalidation skipping, address
remapping and shadow removal hacks still need device tests with a compatible APK.

For each affected game, compare the same scene, save, backend, resolution, driver and
device with one hack off/on. Record frame times, visual differences, audio behavior,
crashes and thermal state; test a cold start and savestate load. Specifically test
game A with an override, game B without one, then A again; restart after changing a
switch. Measure OpenGL and Vulkan independently. See also `ANDROID_TEST.md`.
