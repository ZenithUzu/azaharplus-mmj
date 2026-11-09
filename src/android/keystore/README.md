# Fixed development signing key

`mmj-development.keystore` is a deliberately public development key dedicated to this
fork. The password and key password are `android`; the alias is `androiddebugkey`.
It is not the user's personal debug keystore. Anyone with this repository can sign
with it; do not treat it as a private production identity.

Release builds without an explicit `ANDROID_KEYSTORE_FILE` use this same file on
GitHub Actions and locally. Do not regenerate or replace it for each build. Its
certificate SHA-256 is pinned in `mmj-development.sha256` and checked by CI.

This establishes a stable signing identity for future updates. It does **not** match
the temporary key used for the original `build1`; Android cannot install these APKs
over `build1` while retaining that old signing identity. No signing-key change is
made to an installed app automatically.

A private keystore can still be supplied using the existing signing environment
variables. Changing the signing identity again requires an explicit migration.
