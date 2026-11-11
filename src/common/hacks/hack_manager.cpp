// Copyright 2024-2025 Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#include <algorithm>
#include "hack_manager.h"

namespace Common::Hacks {
std::array<u32, 4> HackManager::GetMmjCpuUsageLimitShifts(u64 title_id, bool enabled) {
    if (!enabled) {
        return {};
    }
    if (OverrideBooleanSetting(HackType::CPU_USAGE_LIMIT_UNIFORM, title_id, false)) {
        return {1, 1, 1, 1};
    }
    if (OverrideBooleanSetting(HackType::CPU_USAGE_LIMIT_HEAVY, title_id, false)) {
        return {2, 4, 2, 2};
    }
    if (OverrideBooleanSetting(HackType::CPU_USAGE_LIMIT, title_id, false)) {
        return {1, 4, 2, 2};
    }
    return {};
}

const HackEntry* HackManager::GetHack(const HackType& type, u64 title_id) {
    auto range = entries.equal_range(type);

    for (auto it = range.first; it != range.second; it++) {
        auto tid_found = std::find(it->second.affected_title_ids.begin(),
                                   it->second.affected_title_ids.end(), title_id);
        if (tid_found != it->second.affected_title_ids.end()) {
            return &it->second;
        }
    }

    return nullptr;
}
} // namespace Common::Hacks
