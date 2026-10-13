// Copyright 2024-2026 Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#pragma once

#include "common/common_types.h"

namespace Common::Hacks {

enum class HackType : int {
    RIGHT_EYE_DISABLE,
    ACCURATE_MULTIPLICATION,
    DECRYPTION_AUTHORIZED,
    ONLINE_LLE_REQUIRED,
    REGION_FROM_SECURE,
    REQUIRES_SHADER_FIXUP,
    SPOOF_FRIEND_CODE_SEED,
    DELAY_TEXTURE_COPY_COMPLETION,
    SKIP_SLOW_DRAW,
    SKIP_TEXTURE_COPY,
    DISABLE_CLIP_COEF,
    SKIP_REDUNDANT_Y2R_CONVERSION,
    FORCE_LINEAR_MAG_FILTER,
    ACCURATE_MULTIPLICATION_MMJ,
    CPU_USAGE_LIMIT,
    CPU_USAGE_LIMIT_HEAVY,
    SKIP_CPU_WRITE,
    DISPLAY_TRANSFER_REMAP,
};

class UserHackData {};

} // namespace Common::Hacks