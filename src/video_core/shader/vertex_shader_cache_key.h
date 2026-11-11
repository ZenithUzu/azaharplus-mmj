// Copyright 2026 Azahar+ mmj contributors
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.txt file included.

#pragma once

#include "common/common_types.h"

namespace VideoCore {

/// Separate runtime shader/pipeline identities for the two multiplication modes.
/// Raw on-disk PICA configuration IDs remain unchanged.
constexpr u64 VertexShaderCacheKey(u64 config_hash, bool accurate_mul) {
    return config_hash ^ (accurate_mul ? 0x9E3779B97F4A7C15ULL : 0ULL);
}

} // namespace VideoCore
