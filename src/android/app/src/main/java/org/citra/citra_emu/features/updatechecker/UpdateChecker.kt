// Copyright 2026 Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

package org.citra.citra_emu.features.updatechecker

import java.net.HttpURLConnection
import java.net.URL
import kotlin.time.Duration.Companion.seconds
import kotlinx.serialization.json.Json
import kotlinx.serialization.json.jsonArray
import kotlinx.serialization.json.jsonObject
import kotlinx.serialization.json.jsonPrimitive
import org.citra.citra_emu.utils.Log

/**
 * Looks for a newer build of Azahar+ mmj in the Releases of this fork only (never in the
 * Azahar or AzaharPlus repositories). CI publishes one release per build, tagged
 * "build-<number>", and passes the same number to the app as BuildConfig.MMJ_BUILD_NUMBER.
 */
object UpdateChecker {
    const val REPO = "axelconde7w7-gif/azaharplus-mmj"
    private val TAG_REGEX = Regex("^build-(\\d+)$")

    data class Release(
        val buildNumber: Int,
        val name: String,
        val pageUrl: String,
        val apkUrl: String?
    )

    private fun getResponse(urlString: String): String? {
        var connection: HttpURLConnection? = null
        try {
            connection = URL(urlString).openConnection() as HttpURLConnection
            connection.requestMethod = "GET"
            connection.setRequestProperty("Accept", "application/vnd.github+json")
            connection.connectTimeout = 15.seconds.inWholeMilliseconds.toInt()
            connection.readTimeout = 15.seconds.inWholeMilliseconds.toInt()

            if (connection.responseCode == HttpURLConnection.HTTP_OK) {
                return connection.inputStream.bufferedReader().use { it.readText() }
            }
            Log.error("[UpdateChecker] HTTP response code ${connection.responseCode}")
            return null
        } catch (e: Exception) {
            Log.error("[UpdateChecker] Request failed: $e")
            return null
        } finally {
            connection?.disconnect()
        }
    }

    /** Returns the newest published build, or null if it cannot be determined. */
    fun getLatestRelease(): Release? {
        val response = getResponse("https://api.github.com/repos/$REPO/releases?per_page=30")
        if (response.isNullOrEmpty()) {
            return null
        }
        return try {
            Json.parseToJsonElement(response).jsonArray
                .map { it.jsonObject }
                .filter { it["draft"]?.jsonPrimitive?.content != "true" }
                .mapNotNull { release ->
                    val tag = release["tag_name"]?.jsonPrimitive?.content ?: return@mapNotNull null
                    val number = TAG_REGEX.matchEntire(tag)?.groupValues?.get(1)?.toIntOrNull()
                        ?: return@mapNotNull null
                    val apk = release["assets"]?.jsonArray
                        ?.map { it.jsonObject }
                        ?.firstOrNull {
                            it["name"]?.jsonPrimitive?.content?.endsWith(".apk") == true
                        }
                        ?.get("browser_download_url")?.jsonPrimitive?.content
                    Release(
                        buildNumber = number,
                        name = release["name"]?.jsonPrimitive?.content ?: tag,
                        pageUrl = release["html_url"]?.jsonPrimitive?.content
                            ?: "https://github.com/$REPO/releases",
                        apkUrl = apk
                    )
                }
                .maxByOrNull { it.buildNumber }
        } catch (e: Exception) {
            Log.error("[UpdateChecker] JSON decode failed: $e")
            null
        }
    }
}
