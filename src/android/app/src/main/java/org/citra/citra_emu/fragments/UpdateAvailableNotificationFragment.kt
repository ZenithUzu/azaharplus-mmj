// Copyright 2026 Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

package org.citra.citra_emu.fragments

import android.app.Dialog
import android.content.DialogInterface
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.fragment.app.DialogFragment
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import org.citra.citra_emu.R
import org.citra.citra_emu.ui.main.MainActivity
import org.citra.citra_emu.utils.BuildUtil

class UpdateAvailableNotificationFragment(
    private val releaseName: String,
    private val apkUrl: String?,
    private val pageUrl: String
) : DialogFragment() {
    private lateinit var mainActivity: MainActivity

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        BuildUtil.assertNotGooglePlay()
        mainActivity = requireActivity() as MainActivity

        isCancelable = false

        return MaterialAlertDialogBuilder(requireContext())
            .setTitle(R.string.update_available)
            .setMessage(getString(R.string.update_available_description, releaseName))
            .setPositiveButton(R.string.update_download) { _: DialogInterface, _: Int ->
                // Opens the APK download (or the release page when there is no APK attached).
                startActivity(Intent(Intent.ACTION_VIEW, Uri.parse(apkUrl ?: pageUrl)))
            }
            .setNeutralButton(R.string.update_release_notes) { _: DialogInterface, _: Int ->
                startActivity(Intent(Intent.ACTION_VIEW, Uri.parse(pageUrl)))
            }
            .setNegativeButton(android.R.string.cancel, null)
            .show()
    }

    companion object {
        const val TAG = "UpdateAvailableNotificationFragment"

        fun newInstance(
            releaseName: String,
            apkUrl: String?,
            pageUrl: String
        ): UpdateAvailableNotificationFragment {
            BuildUtil.assertNotGooglePlay()
            return UpdateAvailableNotificationFragment(releaseName, apkUrl, pageUrl)
        }
    }
}
