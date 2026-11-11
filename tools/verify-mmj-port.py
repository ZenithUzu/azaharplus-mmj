#!/usr/bin/env python3
# Copyright 2026 Azahar+ mmj contributors
# Licensed under GPLv3 or any later version
# Refer to the LICENSE.txt file included.

"""Compare ported title lists and CPU profiles against the pinned MMJ source.

First run: git fetch https://github.com/weihuoya/citra.git master
Then: python tools/verify-mmj-port.py
No network access or repository modifications are performed by this script.
"""

import argparse
from pathlib import Path
import re
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mmj-ref", default="302b0c3bc4312bc96489c11584a97664894b343f")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    source_path = "src/android/app/src/main/assets/config/config-games.ini"
    result = subprocess.run(
        ["git", "show", f"{args.mmj_ref}:{source_path}"], cwd=root,
        capture_output=True, text=True, encoding="utf-8",
    )
    if result.returncode:
        parser.error("MMJ source unavailable; fetch the MMJ remote as described above")

    sections = {}
    section = None
    for line in result.stdout.splitlines():
        line = line.split("//", 1)[0].strip()
        if line.startswith("["):
            section = line[1:-1]
            sections[section] = {}
        elif line and section:
            values = line.split(",")
            shifts = tuple(map(int, values[1:])) if section == "cpu_usage_limit" else ()
            sections[section][int(values[0], 16)] = shifts

    tables = {}
    cpp = (root / "src/common/hacks/hack_list.cpp").read_text(encoding="utf-8")
    cpp = re.sub(r"//[^\n]*", "", cpp)
    for kind, body in re.findall(r"\{HackType::(\w+),\s*HackEntry\{(.*?)\}\}", cpp, re.S):
        ids = re.findall(r"0x([0-9A-Fa-f]{16})\b", body)
        ids = [int(value, 16) for value in ids]
        if len(ids) != len(set(ids)):
            raise SystemExit(f"Duplicate title IDs in {kind}")
        tables.setdefault(kind, set()).update(ids)

    failures = []

    def compare(label, expected, actual):
        missing, extra = set(expected) - set(actual), set(actual) - set(expected)
        if missing or extra:
            failures.append(label)
            print(f"FAIL {label}: missing={sorted(hex(x) for x in missing)}, "
                  f"extra={sorted(hex(x) for x in extra)}")
        else:
            print(f"PASS {label}: {len(expected)} title IDs")

    direct = {
        "skip_texture_copy": "SKIP_TEXTURE_COPY",
        "disable_clip_coef": "DISABLE_CLIP_COEF",
        "use_linear_filter": "FORCE_LINEAR_MAG_FILTER",
        "skip_cpu_write": "SKIP_CPU_WRITE",
        "use_fmv_hack": "FMV_TICKS",
        "disable_shadow_rendering": "DISABLE_SHADOW_RENDERING",
    }
    for source, target in direct.items():
        compare(source, sections[source], tables[target])

    accurate = set(sections["accurate_mul_fast"]) - tables["ACCURATE_MULTIPLICATION"]
    compare("accurate_mul_fast (upstream IDs retained separately)", accurate,
            tables["ACCURATE_MULTIPLICATION_MMJ"])
    # Existing documented exclusion: Azahar fixes the Tales of the Abyss slow-draw crash.
    tales = {0x0004000000061300, 0x000400000004A700, 0x000400000005D700}
    compare("skip_slow_draw (documented Tales exclusion)",
            set(sections["skip_slow_draw"]) - tales, tables["SKIP_SLOW_DRAW"])

    for kind, shifts in {
        "CPU_USAGE_LIMIT": (1, 4, 2, 2),
        "CPU_USAGE_LIMIT_HEAVY": (2, 4, 2, 2),
        "CPU_USAGE_LIMIT_UNIFORM": (1, 1, 1, 1),
    }.items():
        expected = {title for title, value in sections["cpu_usage_limit"].items() if value == shifts}
        compare(f"cpu_usage_limit {shifts}", expected, tables[kind])

    compare("cpu_usage_limit (all profiles covered)", sections["cpu_usage_limit"],
            tables["CPU_USAGE_LIMIT"] | tables["CPU_USAGE_LIMIT_HEAVY"] |
            tables["CPU_USAGE_LIMIT_UNIFORM"])

    compare("y2r_perform_hack (MMJ LoadOverrides)",
            {0x00040000000DCA00, 0x00040000000F4000}, tables["SKIP_REDUNDANT_Y2R_CONVERSION"])
    compare("display_transfer_hack (MMJ LoadOverrides)",
            {0x0004000000068B00, 0x0004000000061300, 0x000400000004A700, 0x000400000005D700},
            tables["DISPLAY_TRANSFER_REMAP"])
    if failures:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
