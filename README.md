# Azahar+ mmj

> **Azahar+ mmj** es un fork no oficial y experimental de [AzaharPlus](https://github.com/AzaharPlus/AzaharPlus), que a su vez es un fork de
> [Azahar](https://github.com/azahar-emu/azahar). Añade los arreglos por juego que acumuló **Citra MMJ**, con un menú para
> activarlos o desactivarlos uno por uno.
>
> *English summary at the [bottom](#english-summary).*

> [!IMPORTANT]
> **🤖 Este es un fork hecho con IA.** Todo el código y la documentación de los cambios de este fork fueron escritos con
> **Claude (Anthropic)** bajo la dirección del propietario del repositorio. Puede contener errores; no es un proyecto oficial. Los
> créditos completos están en la sección [Créditos](#créditos) y **dentro de la app** en *Acerca de → Créditos y aviso de IA*.

> [!WARNING]
> **Estado: experimental.** El APK de esta rama compila en GitHub Actions y fue **probado en un solo dispositivo** (POCO F7 Pro) con
> unos pocos juegos, sin comparar cada hack por separado: ver [`docs/MMJ_PORT.md`](docs/MMJ_PORT.md#pruebas-reportadas). No hay
> resultados documentados juego por juego. No publica binarios y **no está afiliado ni respaldado** por Azahar, AzaharPlus, Citra, Lime3DS, los
> autores de Citra MMJ ni Nintendo.

> [!NOTE]
> **Dónde está cada cosa.** Este repositorio contiene **dos versiones** del mismo port:
> - rama **`azaharplus-mmj`** (esta): basada en **AzaharPlus**, con el menú de interruptores;
> - rama `master`: basada en **Azahar oficial**, sin menú de interruptores.
>
> (GitHub no permite una segunda copia «fork» de AzaharPlus en la misma cuenta, por eso conviven aquí como ramas.)

## Qué es esto

- **Base:** AzaharPlus, rama `AzaharPlus`, commit [`cf90688`](https://github.com/AzaharPlus/AzaharPlus/commit/cf9068844820f819a6eabbeeeb6a70d546e3e85a) (2026-10-08).
  De ahí vienen sus funciones: compatibilidad con archivos de juego de cualquier fork de Citra, descarga de archivos de sistema,
  soporte para CPUs sin SSE4.2, Android 9, ZipPass, trucos integrados, generación de amiibo y mejor multiplayer con otros forks de
  Citra (según su propio [README](docs/UPSTREAM_README.md); aquí no se han verificado una por una).
- **Lo que añade este fork:** los *hacks* por juego de [Citra MMJ](https://github.com/weihuoya/citra) traducidos a la arquitectura
  actual (`HackManager`), un menú para controlarlos, un tope de **8x** en la resolución interna (Azahar permite hasta 18x) y un log
  que indica qué hacks están activos en cada juego.

## Qué buscamos

Citra MMJ acumuló durante años conocimiento práctico sobre juegos concretos de 3DS (qué títulos necesitan multiplicación precisa
en los shaders, cuáles ganan velocidad al saltarse una copia de texturas lenta, etc.), guardado como listas de *title IDs*.
MMJ dejó de incorporar cambios de Citra en abril de 2020 y no puede fusionarse con el código actual, así que **ese conocimiento
corre riesgo de perderse**. Este proyecto intenta rescatarlo, documentarlo y mantenerlo sobre una base activa, dejando siempre
al usuario la opción de apagar cada hack.

**Principios:** cada hack actúa **solo** sobre los títulos de su lista (cualquier otro juego va igual que en AzaharPlus); todos
son **independientes y reversibles**; y se dice con honestidad qué está verificado y qué no.

**Lo que no buscamos:** reemplazar o competir con Azahar o AzaharPlus, ni enviarles estos cambios como pull request
(ver [Transparencia sobre el uso de IA](#transparencia-sobre-el-uso-de-ia)).

## Cheats de 60 fps (descarga dentro de la app)

En la pantalla de **Cheats** de un juego hay, arriba de la lista, un apartado de **cheats de 60 FPS** que siempre se ve y muestra el ID
del juego. El botón **«Descargar cheats de 60 FPS»** aparece **solo si el ID de título del juego tiene un archivo** en [Reshiban/60FPS-AR-CHEATS-3DS](https://github.com/Reshiban/60FPS-AR-CHEATS-3DS)
(241 IDs en el índice; para cualquier otro juego solo se muestra un aviso con su ID y no hay botón). La app **no incluye ni redistribuye** esos cheats: solo
lleva un índice de IDs y nombres de carpeta, y descarga el archivo de ese juego desde GitHub, de un commit fijo
([`041cfa0`](https://github.com/Reshiban/60FPS-AR-CHEATS-3DS/commit/041cfa08a864565e3026896a2f3287579e6b9195)). Ese repositorio
no declara licencia, por eso no se copian sus archivos aquí.

- Se convierte el formato CTRPF (carpetas y comentarios `{...}`) a cheats Gateway/AR normales; las carpetas pasan a ser prefijo del nombre.
- Se añaden **desactivados**, sin tocar los que ya tengas, y se omiten los que usan opcodes que el emulador no soporta (4 de 1330 en el commit fijo).
- Son códigos hechos por terceros para consolas reales: **no están verificados** aquí y algunos (Pokémon) hacen que el juego vaya más rápido.
- Créditos: base de datos de Reshiban; códigos de Hazerou, lucky, NanashiFinal13 y otros que cada cheat nombra en sus notas.

## Otras mejoras de este fork

- **Base de trucos integrada:** se validó la base de AzaharPlus (696 juegos, 12 617 cheats, 74 141 líneas): el formato es correcto (0 líneas
  inválidas). Se corrigió un defecto del cargador: 198 cheats que repetían nombre con **código distinto** se descartaban en silencio;
  ahora se conservan como «Nombre (2)». Los 511 repetidos exactos se siguen omitiendo. No se corrigió: 105 cheats usan el opcode `DF`, que el motor
  no implementa y que simplemente se ignora.
- **Compartir diagnóstico MMJ** (Ajustes de la pantalla de inicio): comparte un resumen corto del último log (cabecera, hacks activos y
  estadísticas de tirones de shaders/pipelines) en lugar del log completo.
- **Nombre:** la app ahora se llama **Azahar+ mmj**; los enlaces de «Soporte», «Sitio web» y «GitHub» de Acerca de apuntan a este
  repositorio, no a AzaharPlus.

## Menú «Ajustes de Citra MMJ» (Android)

En **Ajustes → Ajustes de Citra MMJ** (en inglés, *Citra MMJ hacks*) hay un interruptor por hack, todos **activados por defecto**
y con opción por juego. Los cambios se aplican al iniciar un juego.

| Interruptor | Hack | IDs de título | Ejemplos |
|---|---|---:|---|
| Multiplicación precisa (lista de MMJ) | `ACCURATE_MULTIPLICATION_MMJ` | 42 | Super Mario 3D Land, Kid Icarus: Uprising, Majora's Mask 3D |
| Omitir dibujados lentos | `SKIP_SLOW_DRAW` | 4 | Dragon Quest Monsters: Joker 3, New Atelier Rorona |
| Omitir copias de texturas | `SKIP_TEXTURE_COPY` | 19 | Mario Kart 7, Metroid Prime: Federation Force |
| Desactivar plano de recorte extra | `DISABLE_CLIP_COEF` | 9 | Star Fox 64, Picross 2 |
| Filtro lineal al ampliar | `FORCE_LINEAR_MAG_FILTER` | 5 | Attack on Titan |
| Omitir conversiones YUV redundantes | `SKIP_REDUNDANT_Y2R_CONVERSION` | 2 | Danball Senki |

### Hacks experimentales (desactivados por defecto)

Además hay tres hacks de MMJ que **no se probaron en este fork** y por eso vienen **apagados**. Al encender cada uno la app muestra
un aviso que explica por qué estaba apagado:

| Interruptor | Hack | IDs de título | Por qué está apagado |
|---|---|---:|---|
| Límite de uso de CPU (MMJ) | `CPU_USAGE_LIMIT` / `CPU_USAGE_LIMIT_HEAVY` | 31 | MMJ lo diseñó sobre su propio planificador de CPU; Azahar reparte el tiempo entre núcleos de otra forma y está ajustado para porciones largas, así que el efecto aquí es desconocido |
| Omitir invalidación por escrituras de CPU (MMJ) | `SKIP_CPU_WRITE` | 3 (Lord of Magna) | Escrito para la caché de texturas de MMJ, que Azahar reescribió; puede dejar gráficos viejos |
| Remapeo de direcciones de display transfer (MMJ) | `DISPLAY_TRANSFER_REMAP` | 4 (Tales of the Abyss, Pac-Man Party 3D) | Direcciones fijas copiadas de MMJ, sin verificar con esos juegos; iba unido a un arreglo de cierre inesperado que Azahar corrigió después |

Apagar un interruptor deja esos juegos exactamente como los trata AzaharPlus sin el port. Detalle, notas de fidelidad, lo que **no**
se portó y por qué: **[`docs/MMJ_PORT.md`](docs/MMJ_PORT.md)**.

## Procedencia: de dónde sale cada dato

GitHub solo permite un repositorio padre por fork (aquí, AzaharPlus), así que la relación con MMJ se documenta con enlaces
permanentes a sus commits originales:

| Dato | Primer commit de MMJ donde aparece | Autor | Fecha |
|---|---|---|---|
| `skip_slow_draw` | [`e85a43c`](https://github.com/weihuoya/citra/commit/e85a43cf92e446b4ac4af99a22b53655ea12b92e) | weihuoya | 2020-07-06 |
| `disable_clip_coef` | [`0467d74`](https://github.com/weihuoya/citra/commit/0467d74465a73aba30b8ab712243f9f14b7bcb2d) | weihuoya | 2020-09-21 |
| `y2r_perform_hack` | [`965594b`](https://github.com/weihuoya/citra/commit/965594b140bf6c5c3f5caf6a674d5d3a8a44f5a7) | weihuoya | 2020-12-25 |
| `use_linear_filter` | [`21c231f`](https://github.com/weihuoya/citra/commit/21c231fbc2e1d1523a250330b2543b3c96c0875f) | weihuoya | 2021-01-18 |
| `skip_texture_copy` | [`3c93fb4`](https://github.com/weihuoya/citra/commit/3c93fb489497dd49b58afa5187e735609b149a53) | weihuoya | 2022-01-20 |
| listas completas de IDs (`config-games.ini`) | creado en [`79564b5`](https://github.com/weihuoya/citra/commit/79564b5f34fec812c6750b7165099023f83086a7); [versión usada aquí](https://github.com/weihuoya/citra/blob/302b0c3bc4312bc96489c11584a97664894b343f/src/android/app/src/main/assets/config/config-games.ini) | weihuoya / zhangwei | 2022-04-19 / 2025-11-14 |

Los mensajes de MMJ son muy breves (`fx`, `cache`), así que un commit puede agrupar otros cambios: la tabla indica dónde aparece
**por primera vez** el nombre del ajuste. Historial completo en local: `git remote add mmj https://github.com/weihuoya/citra.git`.

## Compilar y probar

- Guía completa (Android Studio, GitHub Actions, qué mirar en cada juego): **[`docs/ANDROID_TEST.md`](docs/ANDROID_TEST.md)**.
- El APK de este fork usa el sufijo `.mmj` en su ID de aplicación para **instalarse junto a otras versiones** (Azahar, AzaharPlus):
  van firmadas con claves distintas y no pueden reemplazarse entre sí.
- Comprobar que el APK es esta versión: la resolución interna no pasa de 8x, existe el menú de Citra MMJ y el log de cada juego
  incluye una línea `Per-game hacks for <ID>: ...`.

## Transparencia sobre el uso de IA

El código y la documentación de los cambios de este fork fueron escritos con **Claude (Anthropic)**, siguiendo la dirección y
las decisiones del propietario del repositorio; los commits lo declaran con un trailer `Co-Authored-By`. Consecuencias:

- Azahar publica una [política de uso de IA](https://github.com/azahar-emu/azahar/blob/master/AI-POLICY.md) que **prohíbe enviar
  como contribución código escrito total o sustancialmente por IA**. Por eso **este fork no se propone como pull request ni a
  Azahar ni a AzaharPlus**, y se pide que nadie lo haga en su nombre.
- Una IA puede equivocarse: **revisa el diff antes de confiar en él**, sobre todo en `video_core`.

## Créditos

Casi todo lo valioso aquí es trabajo de otras personas:

- **[Azahar Emulator Project](https://github.com/azahar-emu/azahar)** y quienes lo desarrollan: la base de todo, incluido el
  `HackManager` que hace posible el enfoque por título. El logotipo de Azahar es propiedad de PabloMK7 y angyartanddraw (ver
  `the_azahar_logo_is_the_property_of_PabloMK7_and_angyartanddraw.txt`).
- **[AzaharPlus](https://github.com/AzaharPlus/AzaharPlus)** y su equipo (la página no nombra a las personas responsables): la base
  directa de este fork y las funciones extra que ofrece.
- **Citra Emulator Project** y sus colaboradores, creadores del emulador original del que descienden Azahar, Lime3DS y Citra MMJ.
- **Lime3DS** y sus colaboradores. Azahar ha tomado su relevo (conserva el ID de Play Store de Lime3DS y migra datos de Lime3DS y Citra).
- **[weihuoya](https://github.com/weihuoya/citra)**, autor de **Citra MMJ** entre 2019 y 2022 (94 commits propios desde abril de
  2020). Los hacks, los *title IDs* y las observaciones por juego que este fork traduce son fruto **de su trabajo** y de sus
  pruebas con juegos reales.
- **zhangwei**, que mantuvo Citra MMJ de 2023 a 2025 (23 commits, incluido el último) y cuya versión de `config-games.ini` es la
  fuente de datos usada aquí.
- **Claude (Anthropic)**, la IA que escribió el código y la documentación de los cambios de este fork, y **[axelconde7w7-gif](https://github.com/axelconde7w7-gif)**, quien dirige el proyecto, decide qué se hace y prueba los resultados en dispositivo.
- **[Reshiban](https://github.com/Reshiban/60FPS-AR-CHEATS-3DS)** y los autores de los *cheats* de 60 fps que la app descarga de su repositorio (base de datos de Reshiban, basada en el hilo de GBAtemp de **Hazerou** y en la lista de **lucky**; además NanashiFinal13, Spooky y otros que cada cheat nombra). No se redistribuyen: se descargan bajo demanda.
- El equipo de **AzaharPlus** por la base de cheats integrada que ya trae la app.
- Los autores de las bibliotecas externas incluidas (Dynarmic, libzip y otras), con sus propias licencias.

Si eres autor de alguno de estos proyectos y quieres que se corrija o retire algo, abre un issue en este repositorio.

## Licencia

Azahar, AzaharPlus, Citra y Citra MMJ se distribuyen bajo licencias GPL. Cada archivo conserva su aviso de copyright y su licencia
original (la mayoría **GPLv2 o posterior**; algunos archivos más recientes, **GPLv3 o posterior**). Consulta las cabeceras de cada
archivo, [`LICENSE.txt`](LICENSE.txt) y [`misc/licenses/`](misc/licenses/). Los cambios de este fork se publican bajo los mismos términos.

## Reportar problemas

- **Problemas de este fork** (un hack que rompe un juego, un error de compilación): abre un issue **aquí**. Indica juego, ID de
  título, teléfono y, si puedes, el log.
- **No** reportes en los repositorios de Azahar ni de AzaharPlus fallos observados con compilaciones de este fork.

## Aviso legal

Este proyecto es un emulador y no incluye juegos, claves ni firmware de Nintendo, ni ayuda a obtenerlos. Úsalo únicamente con
copias de juegos que hayas obtenido legalmente de tu propia consola. Nintendo y Nintendo 3DS son marcas de Nintendo.

---

## English summary

**Azahar+ mmj: unofficial, experimental fork of [AzaharPlus](https://github.com/AzaharPlus/AzaharPlus)** (itself a fork of Azahar, a Nintendo 3DS
emulator) that ports the per-game compatibility hacks accumulated in **[Citra MMJ](https://github.com/weihuoya/citra)**
(maintained by *weihuoya* until 2022 and by *zhangwei* from 2023 to 2025) onto the current `HackManager`.

- **Menu:** *Settings → Citra MMJ hacks* (Android) has one switch per hack, all on by default and switchable per game. Each hack
  only affects the title IDs listed for it; any other game behaves exactly like AzaharPlus.
- **Also:** internal resolution is capped at 8x, and the log prints which hacks are active when a game starts.
- **Status:** the APK of this branch builds on GitHub Actions and was **tested on one device only** (POCO F7 Pro) with a few games,
  without A/B comparisons per hack; see [`docs/MMJ_PORT.md`](docs/MMJ_PORT.md#pruebas-reportadas). No per-game results yet. No binaries are published; not affiliated with any of the
  projects credited above or with Nintendo.
- **Experimental hacks (off by default):** three more MMJ hacks (CPU usage limit, skip CPU-write invalidation, display-transfer remap)
  are included but switched off, because they were not tested here; turning each one on shows a warning explaining why.
- **60 FPS cheats:** the cheats screen offers an in-app download of the community 60 FPS cheats from
  [Reshiban/60FPS-AR-CHEATS-3DS](https://github.com/Reshiban/60FPS-AR-CHEATS-3DS), only for games whose title ID has a file there.
  Nothing is bundled; the file is downloaded on demand from a pinned commit and added switched off. Credits: Reshiban, Hazerou, lucky, NanashiFinal13 and others.
- **Startup notice:** the app shows a dialog at launch saying it is an unofficial fork made with AI (can be dismissed for good).
- **Made with AI:** this is an AI-made fork (see the in-app *About → Credits and AI notice*). The changes were written with Claude (Anthropic) under the owner's direction. Azahar's
  [AI policy](https://github.com/azahar-emu/azahar/blob/master/AI-POLICY.md) forbids submitting substantially AI-written code, so
  **this fork is not intended as a pull request to Azahar or AzaharPlus**.
- **Credits:** Azahar Emulator Project, AzaharPlus, Citra, Lime3DS, and above all **weihuoya and zhangwei (Citra MMJ)**, whose
  per-game research this fork adapts. Provenance links to the original MMJ commits are in the
  [Procedencia](#procedencia-de-dónde-sale-cada-dato) section.
- **Details:** [`docs/MMJ_PORT.md`](docs/MMJ_PORT.md), [`docs/ANDROID_TEST.md`](docs/ANDROID_TEST.md), original README in
  [`docs/UPSTREAM_README.md`](docs/UPSTREAM_README.md).
