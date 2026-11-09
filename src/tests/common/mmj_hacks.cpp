// Copyright 2026 Azahar+ mmj contributors
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.txt file included.

#include <set>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include "common/hacks/hack_manager.h"

using namespace Common::Hacks;

TEST_CASE("MMJ CPU profiles preserve every per-core shift", "[common][mmj]") {
    const std::array<std::pair<HackType, std::array<u32, 4>>, 3> profiles{{
        {HackType::CPU_USAGE_LIMIT, {1, 4, 2, 2}},
        {HackType::CPU_USAGE_LIMIT_HEAVY, {2, 4, 2, 2}},
        {HackType::CPU_USAGE_LIMIT_UNIFORM, {1, 1, 1, 1}},
    }};
    std::set<u64> titles;
    for (const auto& [type, expected] : profiles) {
        const auto [begin, end] = hack_manager.entries.equal_range(type);
        for (auto entry = begin; entry != end; ++entry) {
            for (const u64 title : entry->second.affected_title_ids) {
                INFO("Title " << title);
                REQUIRE(titles.insert(title).second);
                REQUIRE(hack_manager.GetMmjCpuUsageLimitShifts(title, true) == expected);
                REQUIRE(hack_manager.GetMmjCpuUsageLimitShifts(title, false) ==
                        std::array<u32, 4>{});
            }
        }
    }
    REQUIRE(titles.size() == 35);
    REQUIRE(hack_manager.GetMmjCpuUsageLimitShifts(0, true) == std::array<u32, 4>{});
    // Pokemon Omega Ruby has no CPU profile, even if the switch is enabled.
    REQUIRE(hack_manager.GetMmjCpuUsageLimitShifts(0x000400000011C400, true) ==
            std::array<u32, 4>{});
}

TEST_CASE("MMJ title tables contain unique forced overrides", "[common][mmj]") {
    const std::array<std::pair<HackType, std::size_t>, 13> expected_counts{{
        {HackType::SKIP_SLOW_DRAW, 4},
        {HackType::SKIP_TEXTURE_COPY, 19},
        {HackType::DISABLE_CLIP_COEF, 9},
        {HackType::SKIP_REDUNDANT_Y2R_CONVERSION, 2},
        {HackType::FORCE_LINEAR_MAG_FILTER, 5},
        {HackType::ACCURATE_MULTIPLICATION_MMJ, 42},
        {HackType::CPU_USAGE_LIMIT, 20},
        {HackType::CPU_USAGE_LIMIT_HEAVY, 11},
        {HackType::CPU_USAGE_LIMIT_UNIFORM, 4},
        {HackType::SKIP_CPU_WRITE, 3},
        {HackType::DISPLAY_TRANSFER_REMAP, 4},
        {HackType::FMV_TICKS, 10},
        {HackType::DISABLE_SHADOW_RENDERING, 4},
    }};
    for (const auto& [type, count] : expected_counts) {
        INFO("Hack " << static_cast<int>(type));
        std::set<u64> titles;
        const auto [begin, end] = hack_manager.entries.equal_range(type);
        for (auto entry = begin; entry != end; ++entry) {
            REQUIRE(entry->second.mode == HackAllowMode::FORCE);
            for (const u64 title : entry->second.affected_title_ids) {
                REQUIRE(titles.insert(title).second);
                REQUIRE(hack_manager.OverrideBooleanSetting(type, title, false));
            }
        }
        REQUIRE(titles.size() == count);
        REQUIRE_FALSE(hack_manager.OverrideBooleanSetting(type, 0, false));
        // The owner's first runtime target: MMJ has no active overrides for Ultra Sun.
        REQUIRE_FALSE(hack_manager.OverrideBooleanSetting(type, 0x00040000001B5000, false));
    }
}

TEST_CASE("MMJ does not replace upstream accurate multiplication", "[common][mmj]") {
    constexpr u64 ocarina = 0x0004000000033500;
    constexpr u64 majoras_mask = 0x0004000000125500;
    REQUIRE(hack_manager.OverrideBooleanSetting(HackType::ACCURATE_MULTIPLICATION, ocarina, false));
    REQUIRE_FALSE(
        hack_manager.OverrideBooleanSetting(HackType::ACCURATE_MULTIPLICATION_MMJ, ocarina, false));
    REQUIRE(hack_manager.OverrideBooleanSetting(HackType::ACCURATE_MULTIPLICATION_MMJ, majoras_mask,
                                                false));
}
