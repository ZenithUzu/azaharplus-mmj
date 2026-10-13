// Copyright 2026 Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.txt file included.

package org.citra.citra_emu.features.cheats.model

import android.content.Context
import java.net.HttpURLConnection
import java.net.URL
import java.net.URLEncoder
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import org.json.JSONObject

/**
 * Downloads the community 60 FPS cheats for the running game.
 *
 * The cheats are NOT bundled with the app. They live in
 * https://github.com/Reshiban/60FPS-AR-CHEATS-3DS (based on the work of Hazerou, lucky,
 * NanashiFinal13 and other authors listed in each file). The app only ships a small index of title
 * IDs, used to offer the download for the games that have a file there and for no other game. The
 * file itself is fetched from GitHub on demand, from a fixed commit, and converted from the CTRPF
 * layout used there (folders and {comments}) into plain Gateway/AR cheats this emulator understands.
 */
object Fps60Cheats {
    const val REPO = "Reshiban/60FPS-AR-CHEATS-3DS"
    private const val MAX_BYTES = 1_000_000

    data class Entry(val name: String, val notes: String, val code: String)

    data class ParseResult(val entries: List<Entry>, val incompatible: Int)

    data class ImportResult(val added: Int, val duplicates: Int, val incompatible: Int)

    private var cachedIndex: JSONObject? = null

    private fun index(context: Context): JSONObject? {
        cachedIndex?.let { return it }
        return try {
            val text = context.assets.open("fps60_index.json").bufferedReader()
                .use { it.readText() }
            JSONObject(text).also { cachedIndex = it }
        } catch (e: Exception) {
            null
        }
    }

    private fun idString(titleId: Long) = String.format("%016X", titleId)

    /** Folders of the cheat repository that hold a file for this exact title ID (empty if none). */
    fun foldersFor(context: Context, titleId: Long): List<String> {
        val games = index(context)?.optJSONObject("games") ?: return emptyList()
        val arr = games.optJSONArray(idString(titleId)) ?: return emptyList()
        return List(arr.length()) { arr.getString(it) }
    }

    fun isAvailable(context: Context, titleId: Long) = foldersFor(context, titleId).isNotEmpty()

    fun commit(context: Context): String = index(context)?.optString("commit").orEmpty()

    /** Downloads and converts the cheats for [titleId]. Must not be called on the main thread. */
    suspend fun download(context: Context, titleId: Long): ParseResult =
        withContext(Dispatchers.IO) { downloadBlocking(context, titleId) }

    private fun downloadBlocking(context: Context, titleId: Long): ParseResult {
        val commit = commit(context)
        require(commit.isNotEmpty())
        val source =
            "Source: $REPO @ ${commit.take(8)}. Third-party code, not verified by this fork."
        val entries = ArrayList<Entry>()
        var incompatible = 0
        for (folder in foldersFor(context, titleId)) {
            val path = URLEncoder.encode(folder, "UTF-8").replace("+", "%20")
            val url = URL(
                "https://raw.githubusercontent.com/$REPO/$commit/Cheats/$path/" +
                    "${idString(titleId)}.txt"
            )
            val conn = url.openConnection() as HttpURLConnection
            try {
                conn.connectTimeout = 10_000
                conn.readTimeout = 15_000
                if (conn.responseCode != 200) {
                    throw java.io.IOException("HTTP ${conn.responseCode}")
                }
                val bytes = conn.inputStream.use { input ->
                    val out = java.io.ByteArrayOutputStream()
                    val buffer = ByteArray(8192)
                    while (out.size() < MAX_BYTES) {
                        val n = input.read(buffer)
                        if (n < 0) break
                        out.write(buffer, 0, n)
                    }
                    out.toByteArray()
                }
                val parsed = Fps60Parser.parse(String(bytes, Charsets.UTF_8), source)
                entries += parsed.entries
                incompatible += parsed.incompatible
            } finally {
                conn.disconnect()
            }
        }
        return ParseResult(entries, incompatible)
    }
}
