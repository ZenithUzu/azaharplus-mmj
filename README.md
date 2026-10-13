<p align="center">
  <img src="docs/img/azahar-plus-mmj-logo.jpg" alt="Azahar+ mmj logo" width="520">
</p>

# Azahar+ mmj

Azahar+ mmj is an unofficial Nintendo 3DS emulator for Android (the desktop sources are still in the tree). It carries over the
per-game fixes that [Citra MMJ](https://github.com/weihuoya/citra) collected over the years onto the current code of
[Azahar](https://github.com/azahar-emu/azahar) and [AzaharPlus](https://github.com/AzaharPlus/AzaharPlus).

It is not affiliated with, or endorsed by, Azahar, AzaharPlus, Citra, Lime3DS, the Citra MMJ authors or Nintendo.

- [Which branch to use](#which-branch-to-use)
- [Status](#status)
- [Why this project exists](#why-this-project-exists)
- [What `master` contains](#what-master-contains)
- [What `azaharplus-mmj` adds](#what-azaharplus-mmj-adds)
- [Where the data comes from](#where-the-data-comes-from)
- [Authorship and AI use](#authorship-and-ai-use)
- [Credits](#credits)
- [License](#license)
- [Reporting problems](#reporting-problems)

## Which branch to use

This repository holds two lines of the same port.

| Branch | Based on | What it is |
|---|---|---|
| [`azaharplus-mmj`](../../tree/azaharplus-mmj) | AzaharPlus | The maintained line. It has the settings menu, experimental hacks, 60 FPS cheat download, update checker and release builds. Start here. |
| `master` (this page) | Azahar | The first version of the port. The hacks are always on for their listed games and there is no settings menu. |

Both live here as branches because GitHub only allows one fork of a given repository per account. The README of the
[`azaharplus-mmj` branch](../../tree/azaharplus-mmj#readme) describes the maintained line in full. APKs are published on the
[Releases page](../../releases) and come from that branch.

## Status

Experimental. The `master` code compiles and builds an APK, and the AzaharPlus-based line has been run on a single Android device
with a handful of games. There are no per-hack A/B measurements and no game-by-game results, so nothing here should be taken as
verified compatibility. What has and has not been checked is written down in [`docs/MMJ_PORT.md`](docs/MMJ_PORT.md).

## Why this project exists

Citra MMJ accumulated practical knowledge about specific 3DS games: which titles need accurate shader multiplication, which ones
run faster if a slow texture copy is skipped, which ones crash unless a draw is dropped. That knowledge is stored as lists of
title IDs. MMJ stopped merging upstream Citra changes in April 2020 and cannot be merged into current code, so the lists were at
risk of being lost with it.

This project re-implements those hacks on top of Azahar's `HackManager`, keeps the title ID lists and documents what each one
does. Three rules apply to all of them:

- A hack only acts on the titles in its own list. Every other game runs as it does upstream.
- Each hack is independent of the others.
- Anything that has not been verified is labeled as unverified.

It is not meant to replace Azahar or AzaharPlus, and it is not offered to either project as a pull request (see
[Authorship and AI use](#authorship-and-ai-use)).

## What `master` contains

On top of upstream Azahar:

| Change | Detail |
|---|---|
| MMJ per-game hacks | Ported to `HackManager`, always active for the titles listed below. |
| Accurate multiplication list | The list of titles that force accurate shader multiplication is extended with MMJ's list, for 57 title IDs in total. On Android the global setting is off by default, so for these titles the list is what turns it on. |
| Resolution cap | Internal resolution is limited to 8x (Azahar allows up to 18x). |
| Hack log line | Each game start logs `Per-game hacks for <title ID>: ...` with the state of every hack, so you can confirm which ones were active. |
| Android APK workflow | `.github/workflows/android-apk.yml` builds an arm64 APK. It is disabled on this branch; builds come from `azaharplus-mmj`. |

| Hack | `HackType` | Title IDs | Example games |
|---|---|---:|---|
| Skip slow draws | `SKIP_SLOW_DRAW` | 4 | Dragon Quest Monsters: Joker 3, New Atelier Rorona |
| Skip texture copies | `SKIP_TEXTURE_COPY` | 19 | Mario Kart 7, Metroid Prime: Federation Force |
| Disable extra clip plane | `DISABLE_CLIP_COEF` | 9 | Star Fox 64, Picross 2 |
| Linear filter on magnification | `FORCE_LINEAR_MAG_FILTER` | 5 | Attack on Titan |
| Skip redundant YUV conversions | `SKIP_REDUNDANT_Y2R_CONVERSION` | 2 | Danball Senki |
| Accurate multiplication | `ACCURATE_MULTIPLICATION` | 57 | Zelda: Ocarina of Time 3D, Majora's Mask 3D, Super Mario 3D Land, Kid Icarus: Uprising |

The hacks live in code shared by the OpenGL and Vulkan renderers, so they apply to both backends. That is based on reading the
code and has not been checked game by game.

Hacks that were not ported, and why, are listed in [`docs/MMJ_PORT.md`](docs/MMJ_PORT.md). Most of them either depend on MMJ
internals that Azahar rewrote, or are flags that nothing in MMJ reads.

## What `azaharplus-mmj` adds

The maintained line is AzaharPlus plus everything above, and in addition:

| Feature | What it does |
|---|---|
| Hack switches | One setting per hack under *Settings → Citra MMJ hacks*, with per-game overrides. |
| Experimental hacks | Five more MMJ hacks (CPU usage limit, skip invalidation on CPU writes, display transfer remap, minimum CPU ticks, disable shadow rendering), off by default. Turning one on shows a dialog explaining why it was off. |
| 60 FPS cheat download | A section in a game's cheat screen. The download button only appears when the game's title ID has a file in the cheat repository. |
| Built-in cheat database fix | Cheats that shared a name but had different codes were silently dropped by the loader. They are now kept. |
| MMJ diagnostics | A short summary of the last log with the active hacks and shader or pipeline stall measurements, for bug reports. |
| Release-based updates | The app checks this repository's Releases only. The Azahar and AzaharPlus update notices were removed. |
| Fork notice and credits | A dialog at first launch and a credits screen state that this is an unofficial fork made with AI. |
| Name and icon | Renamed to Azahar+ mmj with its own icon. |

## Where the data comes from

GitHub allows a single parent per fork, so the link to MMJ is kept as permanent links to the original commits.

| Data | First MMJ commit where it appears | Author | Date |
|---|---|---|---|
| `skip_slow_draw` | [`e85a43c`](https://github.com/weihuoya/citra/commit/e85a43cf92e446b4ac4af99a22b53655ea12b92e) | weihuoya | 2020-07-06 |
| `disable_clip_coef` | [`0467d74`](https://github.com/weihuoya/citra/commit/0467d74465a73aba30b8ab712243f9f14b7bcb2d) | weihuoya | 2020-09-21 |
| `y2r_perform_hack` | [`965594b`](https://github.com/weihuoya/citra/commit/965594b140bf6c5c3f5caf6a674d5d3a8a44f5a7) | weihuoya | 2020-12-25 |
| `use_linear_filter` | [`21c231f`](https://github.com/weihuoya/citra/commit/21c231fbc2e1d1523a250330b2543b3c96c0875f) | weihuoya | 2021-01-18 |
| `skip_texture_copy` | [`3c93fb4`](https://github.com/weihuoya/citra/commit/3c93fb489497dd49b58afa5187e735609b149a53) | weihuoya | 2022-01-20 |
| Full ID lists (`config-games.ini`) | created in [`79564b5`](https://github.com/weihuoya/citra/commit/79564b5f34fec812c6750b7165099023f83086a7); [version used here](https://github.com/weihuoya/citra/blob/302b0c3bc4312bc96489c11584a97664894b343f/src/android/app/src/main/assets/config/config-games.ini) | weihuoya / zhangwei | 2022-04-19 / 2025-11-14 |

MMJ commit messages are very short (`fx`, `cache`), so a commit can bundle other changes. The table shows where the setting's name
first appears. To browse the full history locally: `git remote add mmj https://github.com/weihuoya/citra.git`.

## Authorship and AI use

The code and documentation for this project's changes were written with Claude (Anthropic). Commits carry a `Co-Authored-By`
trailer.

Two practical consequences:

- Azahar has an [AI policy](AI-POLICY.md) that forbids submitting code written wholly or substantially by AI. For that reason this
  project is not proposed as a pull request to Azahar or AzaharPlus, and nobody should do so on its behalf.
- AI-written code can be wrong. Read the diff before trusting it, particularly anything under `video_core`.

## Credits

Most of what is useful here is other people's work.

- [Azahar Emulator Project](https://github.com/azahar-emu/azahar) and its developers: the base of everything, including the
  `HackManager` that makes per-title behavior possible.
- [AzaharPlus](https://github.com/AzaharPlus/AzaharPlus) and its team (their page does not name the individuals): the base of the
  maintained line, its extra features and the built-in cheat database.
- Citra Emulator Project and its contributors, who made the emulator Azahar, Lime3DS and Citra MMJ descend from.
- Lime3DS and its contributors. Azahar continues from it.
- [weihuoya](https://github.com/weihuoya/citra), author of Citra MMJ between 2019 and 2022 (94 commits of their own since April
  2020). The hacks, title IDs and per-game observations adapted here come from their work and their testing with real games.
- zhangwei, who maintained Citra MMJ from 2023 to 2025 (23 commits, including the last). Their version of `config-games.ini` is
  the data source used here.
- [Reshiban](https://github.com/Reshiban/60FPS-AR-CHEATS-3DS) and the authors of the 60 FPS cheats that the `azaharplus-mmj`
  branch downloads from that repository: Hazerou, lucky, NanashiFinal13, Spooky and others named in each cheat. They are
  downloaded on demand, not redistributed.
- The authors of the bundled libraries (Dynarmic, libzip and others), under their own licenses.
- Claude (Anthropic) wrote the code and documentation of this project's changes.
- Logo: the flower is Azahar's logo, property of PabloMK7 and angyartanddraw. The logo of the maintained line is a mix of the
  Azahar and Citra MMJ logos made with AI; the author holds no rights over it and it is subject to change. It is used here to
  identify the base of the project and does not imply official endorsement.

If you wrote any of these projects and want something corrected or removed, open an issue here.

## License

Azahar, AzaharPlus, Citra and Citra MMJ are distributed under GPL licenses. Every file keeps its original copyright notice and
license (mostly GPLv2 or later, some newer files GPLv3 or later). See the file headers, [`LICENSE.txt`](LICENSE.txt) and
[`misc/licenses/`](misc/licenses/). Changes in this project are published under the same terms.

## Reporting problems

Problems with this project (a hack that breaks a game, a build failure) go in an issue in this repository. Include the game, its
title ID, the phone and, if possible, the log. Please do not report problems seen with this project to the Azahar or AzaharPlus
repositories.

This project is an emulator and does not include games, keys or Nintendo firmware, and does not help obtain them. Use it only
with copies of games you legally obtained from your own console. Nintendo and Nintendo 3DS are trademarks of Nintendo.
