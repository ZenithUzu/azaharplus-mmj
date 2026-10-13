// Copyright 2026 Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.txt file included.

package org.citra.citra_emu.utils

/** Builds a short report from the emulator log, to make bug reports and stutter tests easy. */
object MmjDiagnostics {
    private const val HEADER_LINES = 14
    private const val MAX_HACK_LINES = 8
    private val GL_STALL =
        Regex("Shader stall: program (\\w+) link=([0-9.]+)ms disk_save=([0-9.]+)ms")
    private val VK_STALL = Regex("Pipeline stall: waited ([0-9.]+)ms")

    fun build(log: List<String>): String {
        val out = StringBuilder()
        out.appendLine("== Azahar+ mmj diagnostics ==")
        out.appendLine("-- Log header --")
        log.take(HEADER_LINES).forEach { out.appendLine(it) }

        val hacks = log.filter { it.contains("Per-game hacks for") || it.contains("MMJ ") }
        out.appendLine("-- Active hacks (last ${MAX_HACK_LINES}) --")
        if (hacks.isEmpty()) out.appendLine("(none logged)")
        hacks.takeLast(MAX_HACK_LINES).forEach { out.appendLine(it) }

        val gl = log.mapNotNull { GL_STALL.find(it) }.map {
            it.groupValues[2].toDouble() + it.groupValues[3].toDouble()
        }
        val vk = log.mapNotNull { VK_STALL.find(it) }.map { it.groupValues[1].toDouble() }
        out.appendLine("-- Stalls (>= 4 ms) --")
        appendStalls(out, "OpenGL shader link + cache save", gl)
        appendStalls(out, "Vulkan pipeline wait", vk)
        return out.toString()
    }

    private fun appendStalls(out: StringBuilder, label: String, values: List<Double>) {
        if (values.isEmpty()) {
            out.appendLine("$label: none")
            return
        }
        val sorted = values.sorted()
        val p95 = sorted[((sorted.size - 1) * 0.95).toInt()]
        out.appendLine(
            "$label: count=${values.size} total=${"%.0f".format(values.sum())}ms " +
                "p95=${"%.1f".format(p95)}ms max=${"%.1f".format(sorted.last())}ms"
        )
    }
}
