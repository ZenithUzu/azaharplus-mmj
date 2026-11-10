// Copyright 2026 Azahar+ mmj contributors
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.txt file included.

#include <array>
#include <vector>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include "common/hacks/hack_manager.h"
#include "video_core/mmj_gpu_settings.h"
#include "video_core/rasterizer_interface.h"

using namespace VideoCore;
using Common::Hacks::hack_manager;
using Common::Hacks::HackType;

namespace {
MmjGpuOptions EnabledOptions() {
    return {false, true, true, true, true, true, true, true, true};
}

// Exercise the actual common rasterizer boundary without requiring a graphics driver.
class RecordingRasterizer final : public RasterizerInterface {
public:
    void AddTriangle(const Pica::OutputVertex&, const Pica::OutputVertex&,
                     const Pica::OutputVertex&) override {}
    void DrawTriangles() override {
        flushed_profiles.push_back(GetMmjSettings());
    }
    void FlushAll() override {}
    void FlushRegion(PAddr, u32) override {}
    void InvalidateRegion(PAddr, u32) override {}
    void FlushAndInvalidateRegion(PAddr, u32) override {}
    void ClearAll(bool) override {}

    std::vector<MmjGpuSettings> flushed_profiles;
    std::vector<MmjGpuSettings> received_profiles;

protected:
    void OnMmjSettingsChanged() override {
        received_profiles.push_back(GetMmjSettings());
    }
};
} // namespace

TEST_CASE("MMJ GPU resolution gates each real title list independently", "[video_core][mmj]") {
    struct Mapping {
        HackType type;
        bool MmjGpuSettings::* resolved;
        bool MmjGpuOptions::* option;
    };
    const std::array mappings{
        Mapping{HackType::ACCURATE_MULTIPLICATION_MMJ, &MmjGpuSettings::accurate_mul,
                &MmjGpuOptions::mmj_accurate_mul},
        Mapping{HackType::SKIP_SLOW_DRAW, &MmjGpuSettings::skip_slow_draw,
                &MmjGpuOptions::skip_slow_draw},
        Mapping{HackType::SKIP_TEXTURE_COPY, &MmjGpuSettings::skip_texture_copy,
                &MmjGpuOptions::skip_texture_copy},
        Mapping{HackType::DISABLE_CLIP_COEF, &MmjGpuSettings::disable_clip_coef,
                &MmjGpuOptions::disable_clip_coef},
        Mapping{HackType::FORCE_LINEAR_MAG_FILTER, &MmjGpuSettings::force_linear_mag_filter,
                &MmjGpuOptions::force_linear_mag_filter},
        Mapping{HackType::SKIP_CPU_WRITE, &MmjGpuSettings::skip_cpu_write,
                &MmjGpuOptions::skip_cpu_write},
        Mapping{HackType::DISPLAY_TRANSFER_REMAP, &MmjGpuSettings::display_transfer_remap,
                &MmjGpuOptions::display_transfer_remap},
        Mapping{HackType::DISABLE_SHADOW_RENDERING, &MmjGpuSettings::skip_shadow_draw,
                &MmjGpuOptions::skip_shadow_draw},
    };
    for (const auto& mapping : mappings) {
        const auto [begin, end] = hack_manager.entries.equal_range(mapping.type);
        REQUIRE(begin != end);
        for (auto it = begin; it != end; ++it) {
            for (const u64 title : it->second.affected_title_ids) {
                INFO("Title " << title << ", hack " << static_cast<int>(mapping.type));
                auto options = EnabledOptions();
                const auto active = ResolveMmjGpuSettings(hack_manager, title, options);
                REQUIRE(active.*mapping.resolved);
                options.*mapping.option = false;
                const auto inactive = ResolveMmjGpuSettings(hack_manager, title, options);
                REQUIRE_FALSE(inactive.*mapping.resolved);
                REQUIRE(ResolveMmjGpuSettings(hack_manager, 0, EnabledOptions()) ==
                        MmjGpuSettings{});
            }
        }
    }
}

TEST_CASE("MMJ GPU resolution retains upstream accuracy and global accuracy", "[video_core][mmj]") {
    REQUIRE(ResolveMmjGpuSettings(hack_manager, 0x0004000000033500, {}).accurate_mul);
    auto options = MmjGpuOptions{};
    options.accurate_mul = true;
    REQUIRE(ResolveMmjGpuSettings(hack_manager, 0x00040000001B5000, options).accurate_mul);
    REQUIRE(ResolveMmjGpuSettings(hack_manager, 0x00040000001B5000, EnabledOptions()) ==
            MmjGpuSettings{});
}

TEST_CASE("MMJ slow-draw skipping cannot blank the dedicated software renderer",
          "[video_core][mmj]") {
    constexpr u64 joker3 = 0x000400000016AD00;
    REQUIRE(ResolveMmjGpuSettings(hack_manager, joker3, EnabledOptions(), true).skip_slow_draw);
    REQUIRE_FALSE(
        ResolveMmjGpuSettings(hack_manager, joker3, EnabledOptions(), false).skip_slow_draw);
}

TEST_CASE("MMJ rasterizer applies a complete profile and flushes under the old profile",
          "[video_core][mmj]") {
    RecordingRasterizer rasterizer;
    const MmjGpuSettings all{true, true, true, true, true, true, true, true};
    rasterizer.ApplyMmjSettings(all);
    REQUIRE(rasterizer.flushed_profiles == std::vector<MmjGpuSettings>{MmjGpuSettings{}});
    REQUIRE(rasterizer.received_profiles == std::vector<MmjGpuSettings>{all});
    REQUIRE(rasterizer.GetMmjSettings() == all);

    // Re-acquiring GPU rights with identical settings must not submit another draw.
    rasterizer.ApplyMmjSettings(all);
    REQUIRE(rasterizer.flushed_profiles.size() == 1);
    REQUIRE(rasterizer.received_profiles.size() == 1);

    // Another process/title still needs an ordered flush even with identical flags.
    rasterizer.ApplyMmjSettings(all, 0x0004000000030100);
    REQUIRE(rasterizer.flushed_profiles.size() == 2);
    REQUIRE(rasterizer.received_profiles.size() == 2);
    REQUIRE(rasterizer.GetMmjTitleId() == 0x0004000000030100);

    rasterizer.ApplyMmjSettings({});
    REQUIRE(rasterizer.flushed_profiles.back() == all);
    REQUIRE(rasterizer.received_profiles.back() == MmjGpuSettings{});
    REQUIRE(rasterizer.GetMmjSettings() == MmjGpuSettings{});

    // A newly constructed renderer receives the full active state again.
    RecordingRasterizer recreated;
    recreated.ApplyMmjSettings(all);
    REQUIRE(recreated.received_profiles == std::vector<MmjGpuSettings>{all});
}

TEST_CASE("MMJ title transitions cannot retain another game's GPU hacks", "[video_core][mmj]") {
    RecordingRasterizer rasterizer;
    for (const u64 title : {0x000400000016AD00ULL, 0x0004000000030400ULL, 0x0004000000030100ULL,
                            0x0004000000120900ULL, 0x0004000000076400ULL, 0x00040000001B5000ULL}) {
        const auto expected = ResolveMmjGpuSettings(hack_manager, title, EnabledOptions());
        rasterizer.ApplyMmjSettings(expected, title);
        REQUIRE(rasterizer.GetMmjSettings() == expected);
        REQUIRE(rasterizer.GetMmjTitleId() == title);
    }
    REQUIRE(rasterizer.GetMmjSettings() == MmjGpuSettings{});
}

TEST_CASE("MMJ clipping and shadow policies preserve ordinary geometry", "[video_core][mmj]") {
    MmjGpuSettings settings;
    REQUIRE(settings.ClipPlaneEnabled(true));
    REQUIRE_FALSE(settings.ClipPlaneEnabled(false));
    REQUIRE_FALSE(settings.SkipShadowDraw(true));
    settings.disable_clip_coef = true;
    settings.skip_shadow_draw = true;
    REQUIRE_FALSE(settings.ClipPlaneEnabled(true));
    REQUIRE_FALSE(settings.ClipPlaneEnabled(false));
    REQUIRE(settings.SkipShadowDraw(true));
    REQUIRE_FALSE(settings.SkipShadowDraw(false));
}

TEST_CASE("MMJ tiny CPU-write policy never suppresses GPU-owned invalidations",
          "[video_core][mmj]") {
    MmjGpuSettings settings;
    REQUIRE_FALSE(settings.SkipCpuWrite(4, false));
    settings.skip_cpu_write = true;
    REQUIRE_FALSE(settings.SkipCpuWrite(0, false));
    for (u32 size = 1; size <= 8; ++size) {
        REQUIRE(settings.SkipCpuWrite(size, false));
        REQUIRE_FALSE(settings.SkipCpuWrite(size, true));
    }
    REQUIRE_FALSE(settings.SkipCpuWrite(9, false));
    REQUIRE_FALSE(settings.SkipCpuWrite(0xFFFFFFFF, false));
}

TEST_CASE("MMJ display-transfer remap is address and height guarded", "[video_core][mmj]") {
    MmjGpuSettings settings;
    REQUIRE(settings.RemapDisplayTransfer(0x183CE430, 400) == 0x183CE430);
    settings.display_transfer_remap = true;
    REQUIRE(settings.RemapDisplayTransfer(0x183CE430, 400) == 0x18300000);
    REQUIRE(settings.RemapDisplayTransfer(0x18387F30, 400) == 0x18346500);
    REQUIRE(settings.RemapDisplayTransfer(0x180B4830, 240) == 0x18080000);
    REQUIRE(settings.RemapDisplayTransfer(0x1807C430, 240) == 0x180B8400);
    REQUIRE(settings.RemapDisplayTransfer(0x183CE430, 240) == 0x183CE430);
    REQUIRE(settings.RemapDisplayTransfer(0x180B4830, 400) == 0x180B4830);
    REQUIRE(settings.RemapDisplayTransfer(0x12345678, 400) == 0x12345678);
}
