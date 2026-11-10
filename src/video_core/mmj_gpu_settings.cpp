// Copyright 2026 Azahar+ mmj contributors
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.txt file included.

#include "common/hacks/hack_manager.h"
#include "video_core/mmj_gpu_settings.h"

namespace VideoCore {

MmjGpuSettings ResolveMmjGpuSettings(Common::Hacks::HackManager& hacks, u64 title_id,
                                     const MmjGpuOptions& options, bool hardware_renderer) {
    using Common::Hacks::HackType;
    const auto enabled = [&](bool option, HackType type) {
        return option && hacks.OverrideBooleanSetting(type, title_id, false);
    };
    return {
        .accurate_mul = hacks.OverrideBooleanSetting(HackType::ACCURATE_MULTIPLICATION, title_id,
                                                     options.accurate_mul) ||
                        enabled(options.mmj_accurate_mul, HackType::ACCURATE_MULTIPLICATION_MMJ),
        .skip_slow_draw =
            hardware_renderer && enabled(options.skip_slow_draw, HackType::SKIP_SLOW_DRAW),
        .skip_texture_copy = enabled(options.skip_texture_copy, HackType::SKIP_TEXTURE_COPY),
        .disable_clip_coef = enabled(options.disable_clip_coef, HackType::DISABLE_CLIP_COEF),
        .force_linear_mag_filter =
            enabled(options.force_linear_mag_filter, HackType::FORCE_LINEAR_MAG_FILTER),
        .skip_cpu_write = enabled(options.skip_cpu_write, HackType::SKIP_CPU_WRITE),
        .display_transfer_remap =
            enabled(options.display_transfer_remap, HackType::DISPLAY_TRANSFER_REMAP),
        .skip_shadow_draw = enabled(options.skip_shadow_draw, HackType::DISABLE_SHADOW_RENDERING),
    };
}

} // namespace VideoCore
