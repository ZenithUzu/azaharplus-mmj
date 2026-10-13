# Probar este fork en Android Studio

## Requisitos (segun src/android/app/build.gradle.kts)
- Android SDK Platform 35, NDK `27.3.13750724`, CMake `3.25.0+` (SDK Manager > SDK Tools), JDK 17.
- ~15 GB libres. Primera compilacion de la parte nativa: larga (decenas de minutos en un portatil).

## Pasos
1. `git clone --recurse-submodules <URL de este repositorio>` (ruta corta en Windows, p. ej. `C:\dev`; antes `git config --global core.longpaths true`)
   (si ya clonaste sin submodulos: `git submodule update --init --recursive`)
2. Abrir **`src/android`** (no la raiz) en Android Studio y esperar el Gradle sync.
3. Para compilar solo lo que usa un telefono, anade `-Pabi=arm64-v8a` en Settings > Build > Gradle (Command-line Options), o al comando de Gradle.
   Para que se instale junto a otras versiones (firma distinta), anade tambien `-PappIdSuffix=.mmj`.
4. Build variant recomendada: **relWithDebInfo** (rinde como release, firma de debug, se instala junto a otras
   versiones con sufijo `.debug`). Terminal: `./gradlew assembleRelWithDebInfo`.

## Como probar cada hack (A/B)
Compara el mismo punto del juego con el interruptor del hack encendido y apagado (Ajustes > Ajustes de Citra MMJ), o contra AzaharPlus sin el port.
| Hack | Juego | Que mirar |
|---|---|---|
| SKIP_TEXTURE_COPY | Mario Kart 7, Kid Icarus: Uprising | rendimiento y que no falten texturas/efectos en menus y pista |
| DISABLE_CLIP_COEF | Star Fox 64, Picross 2 | geometria que desaparece o se recorta mal en los bordes de pantalla |
| SKIP_SLOW_DRAW | DQM Joker 3, New Atelier Rorona, My Hero Academia | modelos o efectos que faltan frente al oficial (esto es lo que el hack omite a proposito) |
| FORCE_LINEAR_MAG_FILTER | Attack on Titan | texturas ampliadas suavizadas en vez de pixeladas |
| SKIP_REDUNDANT_Y2R_CONVERSION | Danball Senki | videos/cinematicas sin congelarse ni parpadear |
| ACCURATE_MULTIPLICATION_MMJ | Kid Icarus: Uprising, Super Mario 3D Land | en Android esta opcion viene desactivada por defecto: con el interruptor encendido se fuerza en estos juegos. Mira geometria o sombreado roto que desaparece |

Si un juego va peor con un hack, apaga su interruptor. Si quieres quitarlo del todo, borra su ID de
`src/common/hacks/hack_list.cpp` (la lista del hack correspondiente) y reconstruye.

## Compilar el APK en GitHub Actions (sin Android Studio)

El repositorio incluye `.github/workflows/android-apk.yml`, que compila un APK de **arm64-v8a** con el sufijo `.mmj` (se
instala junto a otras versiones) y solo se ejecuta en repositorios de la cuenta `axelconde7w7-gif`. No usa secretos; el APK
queda firmado con la clave de depuracion de Gradle.

1. En el repositorio, abre la pestana **Actions** y, si GitHub lo pide, activa los workflows.
2. Elige **android-apk** > **Run workflow** (tambien corre solo al subir a la rama principal).
3. Al terminar, descarga el artefacto `azaharplus-mmj-arm64-<commit>` y descomprime el `.apk`.

Comando equivalente en local: `cd src/android && ./gradlew assembleVanillaRelease -Pabi=arm64-v8a -PappIdSuffix=.mmj`.
Requiere JDK 17, SDK 35, NDK 27.3.13750724 y CMake 3.30.3 (Android Studio los descarga). `minSdk` es 28 (Android 9).

## Comprobar que los hacks estan activos

Al iniciar cada juego, el log incluye una linea como:

    Per-game hacks for 00040000XXXXXXXX: accurate_mul=true skip_slow_draw=false skip_texture_copy=true disable_clip_coef=false force_linear_mag_filter=false

Si el juego esta en alguna lista de `src/common/hacks/hack_list.cpp` y su interruptor esta encendido, el valor sale en `true`.
Ademas, la resolucion interna ya no ofrece mas de 8x, y existe el menu *Ajustes de Citra MMJ*: son las comprobaciones mas rapidas
de que el APK es esta version.
