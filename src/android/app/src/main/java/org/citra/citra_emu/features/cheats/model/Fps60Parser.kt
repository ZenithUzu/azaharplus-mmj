// Copyright 2026 Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.txt file included.

package org.citra.citra_emu.features.cheats.model

/** Pure-Kotlin converter from the CTRPF cheat layout to plain Gateway/AR cheats. */
object Fps60Parser {
    private val CODE_LINE = Regex("^([0-9A-Fa-f]{8})\\s+([0-9A-Fa-f]{8})(\\s*//.*)?$")
    private val FOLDER_OPEN = Regex("^\\+\\+(.*)\\+\\+$")

    /** Opcodes the emulator's Gateway engine implements: 0-C, D0-DD and E. */
    private fun isSupported(line: String): Boolean {
        val c = line[0].uppercaseChar()
        if (c in '0'..'9' || c in 'A'..'C' || c == 'E') return true
        if (c == 'D') return line[1].uppercaseChar() in "0123456789ABCD"
        return false
    }

    /**
     * Converts a CTRPF-style file: "[++Folder++]" ... "[--]" become a prefix of the cheat name,
     * "{...}" blocks (possibly spanning several lines) become notes, and cheats that use opcodes
     * the emulator does not support are dropped and counted.
     */
    fun parse(text: String, sourceNote: String): Fps60Cheats.ParseResult {
        val entries = ArrayList<Fps60Cheats.Entry>()
        var incompatible = 0
        val folders = ArrayList<String>()
        var name: String? = null
        var fullName = ""
        val code = ArrayList<String>()
        val notes = ArrayList<String>()
        var inBrace = false

        fun flush() {
            val n = name
            if (n != null && code.isNotEmpty()) {
                if (code.all { isSupported(it) }) {
                    entries += Fps60Cheats.Entry(
                        fullName,
                        (notes + sourceNote).joinToString("\n"),
                        code.joinToString("\n")
                    )
                } else {
                    incompatible++
                }
            }
            name = null
            code.clear()
            notes.clear()
        }

        for (raw in text.lineSequence()) {
            var line = raw.trim().trimStart('﻿')
            if (inBrace) {
                val end = line.indexOf('}')
                val part = if (end >= 0) line.substring(0, end) else line
                if (part.isNotBlank()) notes += part.trim()
                if (end >= 0) inBrace = false
                continue
            }
            if (line.isEmpty()) continue
            if (line.startsWith("[") && line.endsWith("]")) {
                val title = line.substring(1, line.length - 1).trim()
                val open = FOLDER_OPEN.matchEntire(title)
                when {
                    open != null -> {
                        flush()
                        folders += open.groupValues[1].trim()
                    }
                    title == "--" -> {
                        flush()
                        if (folders.isNotEmpty()) folders.removeAt(folders.size - 1)
                    }
                    else -> {
                        flush()
                        name = title
                        fullName = (folders + title).joinToString(" / ")
                    }
                }
                continue
            }
            if (line.startsWith("{")) {
                line = line.substring(1)
                val end = line.indexOf('}')
                val part = if (end >= 0) line.substring(0, end) else line
                if (part.isNotBlank()) notes += part.trim()
                inBrace = end < 0
                continue
            }
            val m = CODE_LINE.matchEntire(line)
            if (m != null) {
                if (name != null) {
                    code += "${m.groupValues[1].uppercase()} ${m.groupValues[2].uppercase()}"
                }
            } else if (name != null) {
                notes += line
            }
        }
        flush()
        return Fps60Cheats.ParseResult(entries, incompatible)
    }
}
