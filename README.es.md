<p align="center">
  <img src="docs/img/azahar-plus-mmj-logo.jpg" alt="Logotipo de Azahar+ mmj" width="520">
</p>

# Azahar+ mmj

[English](README.md) · **Español**

Azahar+ mmj es un emulador no oficial de Nintendo 3DS para Android (el código de escritorio sigue en el árbol). Es un fork de
[AzaharPlus](https://github.com/AzaharPlus/AzaharPlus), que a su vez es un fork de [Azahar](https://github.com/azahar-emu/azahar).
Existe para conservar los arreglos por juego que [Citra MMJ](https://github.com/weihuoya/citra) fue reuniendo durante años y
para poder activarlos o desactivarlos uno por uno desde los ajustes.

No está afiliado ni respaldado por Azahar, AzaharPlus, Citra, Lime3DS, los autores de Citra MMJ ni Nintendo.

- [Estado](#estado)
- [Por qué existe este fork](#por-qué-existe-este-fork)
- [Qué añade](#qué-añade)
- [Hacks de MMJ](#hacks-de-mmj)
- [Cheats de 60 FPS](#cheats-de-60-fps)
- [Instalación y actualizaciones](#instalación-y-actualizaciones)
- [Compilar](#compilar)
- [De dónde salen los datos](#de-dónde-salen-los-datos)
- [Autoría y uso de IA](#autoría-y-uso-de-ia)
- [Créditos](#créditos)
- [Licencia](#licencia)
- [Reportar problemas](#reportar-problemas)

## Estado

Experimental. El APK de la rama `azaharplus-mmj` compila en GitHub Actions y se ha ejecutado en un solo dispositivo (POCO F7 Pro,
Snapdragon 8 Gen 3) con unos pocos juegos. No hay mediciones A/B por hack ni resultados juego por juego; lo que se ha comprobado y
lo que no está anotado en [`docs/MMJ_PORT.md`](docs/MMJ_PORT.md). Varias cosas de abajo solo se compilaron, no se ejecutaron, y
se indican como tales.

Este repositorio tiene dos líneas del mismo port:

| Rama | Basada en | Notas |
|---|---|---|
| `azaharplus-mmj` | AzaharPlus (`cf90688`, 2026-10-08) | Esta. Tiene el menú de ajustes, el buscador de actualizaciones y la descarga de cheats de 60 FPS. |
| `master` | Azahar | Más antigua, sin menú de ajustes. |

GitHub solo permite un fork de un mismo repositorio por cuenta, por eso las dos conviven aquí como ramas.

## Por qué existe este fork

Citra MMJ acumuló conocimiento práctico sobre juegos concretos de 3DS: qué títulos necesitan multiplicación precisa en los shaders,
cuáles van más rápido si se omite una copia de texturas lenta, cuáles se cierran si no se descarta un dibujado. Ese conocimiento
está guardado como listas de *title IDs*. MMJ dejó de incorporar cambios de Citra en abril de 2020 y no se puede fusionar con el
código actual, así que las listas corrían riesgo de perderse con él.

Este fork reimplementa esos hacks sobre el `HackManager` actual de Azahar, conserva las listas de IDs y documenta qué hace cada
uno. Tres reglas valen para todos:

- Un hack solo actúa sobre los títulos de su propia lista. Cualquier otro juego va igual que en AzaharPlus.
- Cada hack tiene su propio interruptor y se puede apagar sin afectar a los demás.
- Lo que no está verificado se indica como no verificado.

No pretende reemplazar a Azahar ni a AzaharPlus, y no se ofrece a ninguno de los dos como pull request (ver
[Autoría y uso de IA](#autoría-y-uso-de-ia)).

## Qué añade

Además de lo que ya ofrece AzaharPlus (compatibilidad con archivos de juego de otros forks de Citra, descarga de archivos de
sistema, ZipPass, trucos integrados, generación de amiibo, etc., según su propio [README](docs/UPSTREAM_README.md); nada de eso
se ha vuelto a verificar aquí):

| Función | Qué hace | Por qué está |
|---|---|---|
| Interruptores de hacks MMJ | Un ajuste por hack en *Ajustes → Ajustes de Citra MMJ*, con opción por juego. Los cambios se aplican al iniciar un juego. | Para probar cada arreglo y apagarlo por separado. |
| Hacks experimentales | Cinco hacks más, apagados por defecto. Al encender uno aparece un aviso que explica por qué estaba apagado. | No se probaron en este fork, así que no deben funcionar si no los pides. |
| Descarga de cheats de 60 FPS | Un apartado arriba de la pantalla de cheats de cada juego. El botón de descarga solo aparece si el ID de título del juego tiene archivo en el repositorio de cheats. | Los códigos de 60 FPS son por juego. Ofrecerlos para otro título no tiene sentido. |
| Arreglo de la base de trucos integrada | 198 cheats con el mismo nombre pero código distinto se descartaban en silencio al cargar. Ahora se conservan como `Nombre (2)`. | La base de datos es válida; el cargador estaba tirando datos. |
| Diagnóstico MMJ | *Compartir diagnóstico MMJ* en los ajustes de inicio envía un resumen corto del último log: cabecera, hacks activos y mediciones de tirones de shaders y pipelines. | Un log completo es largo y difícil de leer cuando alguien reporta un problema. |
| Medición de tirones | OpenGL registra `Shader stall` y Vulkan `Pipeline stall` cuando un dibujado espera 4 ms o más por un shader o pipeline. | Para medir el tirón de compilación de shaders antes de intentar arreglarlo. |
| Línea de hacks por juego | Cada inicio de juego registra `Per-game hacks for <title ID>: ...` con el estado de cada hack. | Permite confirmar qué hacks estaban activos. |
| Actualizaciones por releases | La app consulta solo los Releases de este repositorio, y la entrada *Actualizaciones* de la pestaña *Opciones* (los tres puntos) busca bajo demanda. | Se quitaron los avisos de actualización de Azahar y AzaharPlus para que esta versión nunca remita a otro proyecto. |
| Aviso de fork y créditos | Un diálogo en el primer arranque indica que es un fork no oficial hecho con IA. *Acerca de → Créditos y aviso de IA* lista a todos los acreditados abajo. | El usuario debe saber qué está ejecutando y a quién agradecer. |
| Tope de resolución | La resolución interna se limita a 8x (Azahar permite hasta 18x). | Heredado del port de MMJ. |
| Nombre e icono | Renombrada a Azahar+ mmj con icono propio. Los enlaces de Acerca de apuntan a este repositorio. | Para no confundirla con las compilaciones originales. |

## Hacks de MMJ

Ruta en Android: *Ajustes → Ajustes de Citra MMJ* (en inglés, *Citra MMJ hacks*). Los interruptores de la primera tabla vienen
encendidos. Apagar uno deja esos juegos exactamente como los trata AzaharPlus sin el port.

| Interruptor | Hack | IDs de título | Juegos de ejemplo |
|---|---|---:|---|
| Multiplicación precisa (lista de MMJ) | `ACCURATE_MULTIPLICATION_MMJ` | 42 | Super Mario 3D Land, Kid Icarus: Uprising, Majora's Mask 3D |
| Omitir dibujados lentos | `SKIP_SLOW_DRAW` | 4 | Dragon Quest Monsters: Joker 3, New Atelier Rorona |
| Omitir copias de texturas | `SKIP_TEXTURE_COPY` | 19 | Mario Kart 7, Metroid Prime: Federation Force |
| Desactivar plano de recorte extra | `DISABLE_CLIP_COEF` | 9 | Star Fox 64, Picross 2 |
| Filtro lineal al ampliar | `FORCE_LINEAR_MAG_FILTER` | 5 | Attack on Titan |
| Omitir conversiones YUV redundantes | `SKIP_REDUNDANT_Y2R_CONVERSION` | 2 | Danball Senki |

Los hacks están en código compartido por los renderizadores OpenGL y Vulkan (y en las capas de CPU y servicios), así que se aplican
a ambos backends. Esto se basa en leer el código; no se ha comprobado juego por juego en cada backend.

### Hacks experimentales (apagados por defecto)

| Interruptor | Hack | IDs de título | Por qué está apagado |
|---|---|---:|---|
| Límite de uso de CPU | `CPU_USAGE_LIMIT`, `CPU_USAGE_LIMIT_HEAVY` | 31 | MMJ lo construyó sobre su propio planificador de CPU. Azahar reparte el tiempo entre núcleos de otra forma, así que el efecto aquí se desconoce. |
| Omitir invalidación por escrituras de CPU | `SKIP_CPU_WRITE` | 3 (Lord of Magna) | Escrito para la caché de texturas de MMJ, que Azahar reescribió después. Puede dejar gráficos viejos en pantalla. |
| Remapeo de direcciones de display transfer | `DISPLAY_TRANSFER_REMAP` | 4 (Tales of the Abyss, Pac-Man Party 3D) | Direcciones fijas copiadas de MMJ y nunca comprobadas con estos juegos. Se añadió junto a un arreglo de cierre inesperado que Azahar ya corrigió. |
| Mínimo de ticks de CPU por vaciado («FMV hack») | `FMV_TICKS` | 10 (Mirror of Fate, Luigi's Mansion: Dark Moon, Zombie Panic in Wonderland DX) | MMJ no documenta qué arregla. AzaharPlus tiene tres backends de CPU que cuentan los ticks de forma distinta. Dynarmic, DynCom y FastInterp están cubiertos, ninguno probado. |
| Desactivar sombras | `DISABLE_SHADOW_RENDERING` | 4 (Luigi's Mansion: Dark Moon) | El código que lee esta lista no está en el código público de MMJ. La implementación copia lo que MMJ hace en GPUs sin soporte de sombras, que es omitir esos dibujados. |

Los hacks que no se portaron, y el motivo de cada uno (por ejemplo `stream_buffer_hack`, que Azahar ya maneja como lo necesitan esos
juegos), están en [`docs/MMJ_PORT.md`](docs/MMJ_PORT.md).

## Cheats de 60 FPS

Abre la pantalla de cheats de un juego. Arriba hay un apartado de 60 FPS que siempre muestra el ID de título. El botón de descarga
aparece solo si ese ID tiene archivo en
[Reshiban/60FPS-AR-CHEATS-3DS](https://github.com/Reshiban/60FPS-AR-CHEATS-3DS) (241 IDs en el índice incluido). Para cualquier otro
juego el apartado solo muestra el ID.

- La app no incluye ni redistribuye esos cheats. Lleva un índice de IDs y nombres de carpeta y descarga el archivo del juego desde
  GitHub, fijado al commit
  [`041cfa0`](https://github.com/Reshiban/60FPS-AR-CHEATS-3DS/commit/041cfa08a864565e3026896a2f3287579e6b9195). Ese repositorio no
  declara licencia, por eso no se copia ninguno de sus archivos aquí.
- El formato CTRPF (carpetas, comentarios `{...}`) se convierte a cheats Gateway/AR normales. Los nombres de carpeta pasan a ser un
  prefijo del nombre del cheat.
- Los cheats se añaden desactivados y no se tocan los que ya tengas. Se omiten los que usan opcodes que el emulador no implementa
  (4 de 1330 en el commit fijado).
- Son códigos de terceros hechos para consolas reales y no se han verificado aquí. Algunos, como los de Pokémon, hacen que el juego
  vaya más rápido.

## Instalación y actualizaciones

Cada compilación correcta en GitHub Actions publica un release llamado `build-<n>`, marcado como pre-release, con el APK adjunto. Se
conservan los 10 más recientes. Descarga el APK desde la
[página de Releases](https://github.com/axelconde7w7-gif/azaharplus-mmj/releases) e instálalo.

El APK usa el sufijo `.mmj` en su ID de aplicación, así que se instala junto a Azahar o AzaharPlus en lugar de reemplazarlos.

Dentro de la app:

- Al abrirse consulta los Releases de este repositorio y ofrece la descarga si hay una compilación más nueva. Se puede apagar en
  *Ajustes → General → Buscar actualizaciones*.
- *Actualizaciones*, al principio de la pestaña *Opciones* de inicio, muestra el número de build instalado y comprueba al momento,
  aunque la búsqueda automática esté apagada.
- Las compilaciones locales (número de build 0) nunca buscan actualizaciones.
- No se instala nada automáticamente. Android pide confirmación cada vez.

Para instalar un build nuevo encima del anterior, el APK debe firmarse siempre con la misma clave. Sin ella, la CI firma con una clave
de depuración temporal, la firma cambia en cada build y Android rechaza la actualización, así que habría que desinstalar antes. Para
fijar una clave, crea un keystore y añade estos secretos del repositorio en *Settings → Secrets and variables → Actions*:

- `ANDROID_KEYSTORE_BASE64`: el archivo `.jks` codificado en base64
- `ANDROID_KEYSTORE_PASS`: la contraseña del keystore (se usa también como contraseña de la clave)
- `ANDROID_KEY_ALIAS`: el alias de la clave

Guarda una copia segura del keystore. Si se pierde, nadie podrá actualizar encima de las instalaciones existentes.

## Compilar

Las instrucciones completas (Android Studio, GitHub Actions, qué mirar en cada juego) están en
[`docs/ANDROID_TEST.md`](docs/ANDROID_TEST.md). En resumen: abre `src/android` en Android Studio, o deja que el workflow de
`.github/workflows/android-apk.yml` compile un APK arm64. Para comprobar que un build es este fork: la resolución interna se detiene
en 8x, existe el menú de Citra MMJ y el log de cada juego tiene una línea `Per-game hacks for <ID>`.

## De dónde salen los datos

GitHub permite un único padre por fork (aquí, AzaharPlus), así que la relación con MMJ se mantiene con enlaces permanentes a los
commits originales.

| Dato | Primer commit de MMJ donde aparece | Autor | Fecha |
|---|---|---|---|
| `skip_slow_draw` | [`e85a43c`](https://github.com/weihuoya/citra/commit/e85a43cf92e446b4ac4af99a22b53655ea12b92e) | weihuoya | 2020-07-06 |
| `disable_clip_coef` | [`0467d74`](https://github.com/weihuoya/citra/commit/0467d74465a73aba30b8ab712243f9f14b7bcb2d) | weihuoya | 2020-09-21 |
| `y2r_perform_hack` | [`965594b`](https://github.com/weihuoya/citra/commit/965594b140bf6c5c3f5caf6a674d5d3a8a44f5a7) | weihuoya | 2020-12-25 |
| `use_linear_filter` | [`21c231f`](https://github.com/weihuoya/citra/commit/21c231fbc2e1d1523a250330b2543b3c96c0875f) | weihuoya | 2021-01-18 |
| `skip_texture_copy` | [`3c93fb4`](https://github.com/weihuoya/citra/commit/3c93fb489497dd49b58afa5187e735609b149a53) | weihuoya | 2022-01-20 |
| Listas completas de IDs (`config-games.ini`) | creado en [`79564b5`](https://github.com/weihuoya/citra/commit/79564b5f34fec812c6750b7165099023f83086a7); [versión usada aquí](https://github.com/weihuoya/citra/blob/302b0c3bc4312bc96489c11584a97664894b343f/src/android/app/src/main/assets/config/config-games.ini) | weihuoya / zhangwei | 2022-04-19 / 2025-11-14 |

Los mensajes de commit de MMJ son muy breves (`fx`, `cache`), así que un commit puede agrupar otros cambios. La tabla indica dónde
aparece por primera vez el nombre del ajuste. Para ver el historial completo en local:
`git remote add mmj https://github.com/weihuoya/citra.git`.

## Autoría y uso de IA

El código y la documentación de los cambios de este fork se escribieron con Claude (Anthropic), siguiendo la dirección y las
decisiones del propietario del repositorio. Los commits llevan un trailer `Co-Authored-By`, y la app lo indica en el primer arranque
y en *Acerca de → Créditos y aviso de IA*.

Dos consecuencias prácticas:

- Azahar tiene una [política de uso de IA](https://github.com/azahar-emu/azahar/blob/master/AI-POLICY.md) que prohíbe enviar código
  escrito total o sustancialmente por IA. Por eso este fork no se propone como pull request a Azahar ni a AzaharPlus, y nadie debería
  hacerlo en su nombre.
- El código escrito por IA puede estar mal. Revisa el diff antes de confiar en él, sobre todo lo que esté en `video_core`.

## Créditos

Casi todo lo útil de aquí es trabajo de otras personas.

- [Azahar Emulator Project](https://github.com/azahar-emu/azahar) y sus desarrolladores: la base de todo, incluido el `HackManager`
  que hace posible el comportamiento por título.
- [AzaharPlus](https://github.com/AzaharPlus/AzaharPlus) y su equipo (su página no nombra a las personas): la base directa de este
  fork, sus funciones extra y la base de trucos integrada.
- Citra Emulator Project y sus colaboradores, que crearon el emulador del que descienden Azahar, Lime3DS y Citra MMJ.
- Lime3DS y sus colaboradores. Azahar continúa desde él.
- [weihuoya](https://github.com/weihuoya/citra), autor de Citra MMJ entre 2019 y 2022 (94 commits propios desde abril de 2020). Los
  hacks, los *title IDs* y las observaciones por juego que se adaptan aquí son fruto de su trabajo y de sus pruebas con juegos reales.
- zhangwei, que mantuvo Citra MMJ de 2023 a 2025 (23 commits, incluido el último). Su versión de `config-games.ini` es la fuente de
  datos usada aquí.
- [Reshiban](https://github.com/Reshiban/60FPS-AR-CHEATS-3DS) y los autores de los cheats de 60 FPS que la app descarga de ese
  repositorio: Hazerou (hilo de GBAtemp), lucky, NanashiFinal13, Spooky y otros que cada cheat nombra. Se descargan bajo demanda, no
  se redistribuyen.
- Los autores de las bibliotecas incluidas (Dynarmic, libzip y otras), con sus propias licencias.
- Claude (Anthropic) escribió el código y la documentación de los cambios de este fork, y
  [axelconde7w7-gif](https://github.com/axelconde7w7-gif) dirige el proyecto, decide qué se hace y lo prueba en dispositivo.
- Logotipo: la flor es el logotipo de Azahar, propiedad de PabloMK7 y angyartanddraw (ver
  `the_azahar_logo_is_the_property_of_PabloMK7_and_angyartanddraw.txt`). Se usa aquí para identificar la base del proyecto y no
  implica respaldo oficial.

Si eres autor de alguno de estos proyectos y quieres que se corrija o retire algo, abre un issue aquí.

## Licencia

Azahar, AzaharPlus, Citra y Citra MMJ se distribuyen bajo licencias GPL. Cada archivo conserva su aviso de copyright y su licencia
original (en su mayoría GPLv2 o posterior, algunos archivos más recientes GPLv3 o posterior). Consulta las cabeceras de los archivos,
[`LICENSE.txt`](LICENSE.txt) y [`misc/licenses/`](misc/licenses/). Los cambios de este fork se publican bajo los mismos términos.

## Reportar problemas

Los problemas de este fork (un hack que rompe un juego, un error de compilación) van en un issue de este repositorio. Indica el
juego, su ID de título, el teléfono y, si puedes, la salida de *Compartir diagnóstico MMJ* o el log. Por favor, no reportes en los
repositorios de Azahar ni de AzaharPlus problemas vistos con este fork.

Este proyecto es un emulador y no incluye juegos, claves ni firmware de Nintendo, ni ayuda a obtenerlos. Úsalo únicamente con copias
de juegos que hayas obtenido legalmente de tu propia consola. Nintendo y Nintendo 3DS son marcas de Nintendo.
