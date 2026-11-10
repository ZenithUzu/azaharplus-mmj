// Copyright 2026 Azahar+ mmj contributors
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.txt file included.

#pragma once

#include "common/common_types.h"

namespace Common::Hacks {
struct HackManager;
}

namespace VideoCore {

struct MmjGpuOptions {
    bool accurate_mul{};
    bool mmj_accurate_mul{};
    bool skip_slow_draw{};
    bool skip_texture_copy{};
    bool disable_clip_coef{};
    bool force_linear_mag_filter{};
    bool skip_cpu_write{};
    bool display_transfer_remap{};
    bool skip_shadow_draw{};
};

/// Resolved per-title state shared by PICA, both hardware rasterizers and their caches.
struct MmjGpuSettings {
    bool accurate_mul{};
    bool skip_slow_draw{};
    bool skip_texture_copy{};
    bool disable_clip_coef{};
    bool force_linear_mag_filter{};
    bool skip_cpu_write{};
    bool display_transfer_remap{};
    bool skip_shadow_draw{};

    bool operator==(const MmjGpuSettings&) const = default;

    constexpr bool ClipPlaneEnabled(bool register_enabled) const {
        return register_enabled && !disable_clip_coef;
    }

    constexpr bool SkipShadowDraw(bool shadow_rendering) const {
        return skip_shadow_draw && shadow_rendering;
    }

    constexpr bool SkipCpuWrite(u32 size, bool has_gpu_owner) const {
        return skip_cpu_write && !has_gpu_owner && size != 0 && size <= 8;
    }

    constexpr PAddr RemapDisplayTransfer(PAddr address, u32 height) const {
        if (!display_transfer_remap) {
            return address;
        }
        if (height == 400) {
            if (address == 0x183CE430) {
                return address - 0xCE430;
            }
            if (address == 0x18387F30) {
                return address - 0x41A30;
            }
        } else {
            if (address == 0x180B4830) {
                return address - 0x34830;
            }
            if (address == 0x1807C430) {
                return address + 0x3BFD0;
            }
        }
        return address;
    }
};

MmjGpuSettings ResolveMmjGpuSettings(Common::Hacks::HackManager& hacks, u64 title_id,
                                     const MmjGpuOptions& options, bool hardware_renderer = true);

} // namespace VideoCore
