# Azahar+ mmj: port de hacks de Citra MMJ a AzaharPlus

Base: AzaharPlus rama `AzaharPlus` @ `cf90688` (2026-10-08). Origen: weihuoya/citra (MMJ) @ `302b0c3` (2025-11-14).
El mismo port se hizo antes sobre Azahar oficial en un fork aparte (`axelconde7w7-gif/azaharplus-mmj`); aqui se reaplico
sobre AzaharPlus (los 10 parches de codigo aplicaron sin conflictos) y se anadieron los interruptores.

## De donde salen los IDs
MMJ guarda los overrides por juego en dos sitios:
1. `LoadOverrides` en `src/core/core.cpp` (lista corta).
2. `src/android/app/src/main/assets/config/config-games.ini` (lista completa, por seccion).

La primera version del port solo uso (1). Ahora las listas se regeneran desde (2).

## Estado de la verificacion
- Compila sobre esta base: los 8 archivos fuente principales afectados (gpu, pica_core, rasterizer_accelerated, gl/vk_rasterizer,
  y2r_u, settings, hack_list), sin errores ni advertencias. El codigo Kotlin/Android de los interruptores no se pudo compilar en el
  entorno donde se escribio.
- El port original sobre Azahar oficial fue compilado y ejecutado en Android por el propietario del fork, que reporta que funciona.
- Este fork sobre AzaharPlus: el APK compila en GitHub Actions y se probo en un dispositivo (ver "Pruebas reportadas"). No hay
  comparativas por hack ni resultados juego por juego.

## Portado
| Hack MMJ | HackType | IDs | Juegos |
|---|---|---|---|
| skip_slow_draw | SKIP_SLOW_DRAW | 4 | New Atelier Rorona, My Hero Academia, DQM Joker 3 (+Professional) |
| skip_texture_copy | SKIP_TEXTURE_COPY | 19 | Mario Kart 7, Kid Icarus: Uprising, Luigi's Mansion: Dark Moon, Metroid Prime: Federation Force, Batman: Arkham Origins Blackgate, Professor Layton and the Miracle Mask (+Plus) |
| disable_clip_coef | DISABLE_CLIP_COEF | 9 | Star Fox 64, Picross 2, Art of Balance |
| use_linear_filter | FORCE_LINEAR_MAG_FILTER | 5 | Attack on Titan 1 / 1 CHAIN / 2 |
| y2r_perform_hack | SKIP_REDUNDANT_Y2R_CONVERSION | 2 | Danball Senki (solo en LoadOverrides) |
| accurate_mul_fast | ACCURATE_MULTIPLICATION_MMJ | 42 (las 15 de Azahar siguen en ACCURATE_MULTIPLICATION: 57 en total) | Attack on Titan, Super Mario 3D Land, Captain Toad, A Link Between Worlds, Majora's Mask 3D, Kid Icarus: Uprising, Ever Oasis. En Android `shaders_accurate_mul` viene **desactivado** por defecto, asi que aqui el hack si tiene efecto; en escritorio el valor por defecto ya es true |

## Interruptores (activar / desactivar)
Cada hack tiene su ajuste, activado por defecto y con opcion por juego. En Android: **Ajustes > Ajustes de Citra MMJ**
(en ingles: *Citra MMJ hacks*). Los cambios se aplican al iniciar un juego.

| Ajuste (`[MMJ]` en config.ini) | Controla |
|---|---|
| `mmj_accurate_mul` | ACCURATE_MULTIPLICATION_MMJ (42 IDs) |
| `mmj_skip_slow_draw` | SKIP_SLOW_DRAW (4 IDs) |
| `mmj_skip_texture_copy` | SKIP_TEXTURE_COPY (19 IDs) |
| `mmj_disable_clip_coef` | DISABLE_CLIP_COEF (9 IDs) |
| `mmj_force_linear_mag_filter` | FORCE_LINEAR_MAG_FILTER (5 IDs) |
| `mmj_skip_redundant_y2r` | SKIP_REDUNDANT_Y2R_CONVERSION (2 IDs) |

Apagar un interruptor deja ese juego exactamente como lo trata AzaharPlus sin el port. Los hacks que ya traia Azahar
(las 15 IDs de ACCURATE_MULTIPLICATION, RIGHT_EYE_DISABLE, etc.) no tienen interruptor.

### Hacks experimentales (apagados por defecto, con aviso al activarlos)
| Ajuste (`[MMJ]` en config.ini) | Controla | IDs | Origen en MMJ | Motivo de que esté apagado |
|---|---|---:|---|---|
| `mmj_cpu_usage_limit` | CPU_USAGE_LIMIT (20 IDs) y CPU_USAGE_LIMIT_HEAVY (11 IDs) | 31 | `core_downcount_hack`: `downcount = slice_length >> shift` por núcleo, con desplazamientos 1,4,2,2 (HEAVY: 2,4,2,2); IDs de la sección `[cpu_usage_limit]` de `config-games.ini` | MMJ lo diseñó sobre su planificador; el comentario de `MAX_SLICE_LENGTH` en `core_timing.h` de Azahar indica que está ajustado para porciones largas y que las cortas obligan a los núcleos a resincronizarse. No hay mediciones. Se omitieron 4 IDs con desplazamientos 1,1,1,1 (The Binding of Isaac: Rebirth, Futuridium EP Deluxe) |
| `mmj_skip_cpu_write` | SKIP_CPU_WRITE | 3 | `skip_cpu_write` en `gl_rasterizer_cache.cpp` de MMJ (escrituras de CPU de 8 bytes o menos) | La caché de texturas de Azahar se reescribió; la ruta equivalente es `RasterizerCache::InvalidateRegion`. Puede dejar texturas desactualizadas. Sin probar |
| `mmj_display_transfer_remap` | DISPLAY_TRANSFER_REMAP | 4 | `display_transfer_hack` (4 direcciones fijas, Tales of the Abyss / Pac-Man Party 3D) | Valores fijos sin verificar con los juegos; en MMJ iba junto a un arreglo de un cierre inesperado que (inferido por coincidencia de síntomas) Azahar corrigió en #7381, así que puede ya no hacer falta |

Nota sobre las fuentes: en el código público de MMJ no se encontró el lector nativo de `config-games.ini`; las listas de IDs se
tomaron de ese archivo (el mismo criterio usado para los demás hacks) y la semántica de cada hack, del código de MMJ.

## Notas de fidelidad
- disable_clip_coef: en el vertex shader normal el efecto es identico al de MMJ. En el camino de geometry shader
  (`EmitVtx`) MMJ ademas omite `gl_ClipDistance[0]` (plano fijo z <= 0); aqui ese plano se mantiene.
- skip_slow_draw: Tales of the Abyss (3 IDs en el ini, 1 en LoadOverrides) se excluye a proposito: la causa del
  crash se corrigio en Azahar (#7381). Inferido por coincidencia de sintomas, no probado.

## Ya existia en Azahar
- y2r_event_delay (FIFA): retraso minimo de 50000 ciclos aplicado siempre.
- Cache de shaders en disco, compilacion asincrona, presentacion asincrona: ajustes existentes.

## Inertes en MMJ (nadie lee el flag: nada que portar)
`accurate_rcp_rsq` (**todos los Pokemon 3DS** y Captain Toad), `accurate_max_min`, `skip_load_buffer`
(Metroid Prime: Federation Force), `merge_framebuffer`, `hw_gs_mode` / `disable_hw_gs` (BIOHAZARD Revelations),
`use_fence_sync`, `use_compatible_mode`. Se declaran e inicializan pero ninguna ruta de video_core/core los consulta.

## No portado, con motivo
- use_fmv_hack (`core_ticks_hack`, 10 IDs: Mirror of Fate, Luigi's Mansion: Dark Moon, Zombie Panic in Wonderland DX): en MMJ fija un minimo de ticks (16000 con JIT, 0xFFFF sin JIT) en `AddTicks` de Dynarmic y en cada SVC del interprete. AzaharPlus tiene tres backends de CPU (Dynarmic, DynCom, FastInterp) que contabilizan los ticks de otra forma; no hay una equivalencia directa y no se pudo comprobar, asi que no se porto.
- stream_buffer_hack (Shovel Knight, Gunvolt, 1001 Spikes, Gravity Falls): en MMJ esta activo por defecto y esos juegos lo *desactivan* (solo cambia cuando se invalida el buffer de texturas de OpenGL). El `gl_stream_buffer.cpp` de Azahar usa mapeo persistente y otra logica, sin equivalente directo.
- disable_shadow_rendering (Luigi's Mansion: Dark Moon): Azahar no tiene ajuste de sombras equivalente.
- new_3ds_mode (41 IDs): Azahar usa is_new_3ds=true por defecto y lo permite fijar por juego.
- game_region_*: Azahar selecciona la region automaticamente.

## Cheats de 60 fps
Ver el README. Detalles: indice `src/android/app/src/main/assets/fps60_index.json` (241 IDs, 257 carpetas; solo IDs y nombres de carpeta,
commit `041cfa08`); conversor probado con Kotlin 2.0.21 sobre los 257 archivos (1330 cheats, 4 omitidos por usar el opcode `DF`, que el motor
Gateway no implementa); URLs comprobadas con una muestra de 15 archivos (incluidos nombres con acentos y apostrofes). La descarga y la interfaz
no se probaron en un dispositivo.

## Titulos populares
| Titulo | En MMJ | En esta rama |
|---|---|---|
| Zelda: Ocarina of Time 3D | accurate_mul_fast (+ region) | ACCURATE_MULTIPLICATION (ya estaba en Azahar) |
| Zelda: Majora's Mask 3D / A Link Between Worlds | accurate_mul_fast (+ region) | ACCURATE_MULTIPLICATION_MMJ |
| Pokemon X/Y, ORAS, Sun/Moon, USUM | solo accurate_rcp_rsq (inerte) | sin hack: MMJ no aporta nada |
| Mario Kart 7 | skip_texture_copy | SKIP_TEXTURE_COPY |
| Super Mario 3D Land | accurate_mul_fast | ACCURATE_MULTIPLICATION_MMJ (+ DELAY_TEXTURE_COPY_COMPLETION de Azahar) |
| Kid Icarus: Uprising | accurate_mul_fast, skip_texture_copy | ACCURATE_MULTIPLICATION_MMJ y SKIP_TEXTURE_COPY |
| Animal Crossing, Smash, Monster Hunter, Kirby | sin entradas | sin hack |

## Pruebas reportadas
Reportadas por el propietario del fork, sin mediciones ni comparativas A/B. Dispositivo: **POCO F7 Pro**, 12 GB de RAM,
Snapdragon 8 Gen 3. APK de la rama `azaharplus-mmj` compilado en GitHub Actions (2026-10-13).

| Juego | Resultado reportado | Hack MMJ implicado |
|---|---|---|
| Pokemon Ruby (con el truco integrado de AzaharPlus) | 60 fps fijos con resolucion interna x4 | ninguno: MMJ no tiene hacks utiles para los Pokemon 3DS, asi que el rendimiento no es atribuible al port |
| The Legend of Zelda: Majora's Mask 3D | 30 fps fijos con resolucion interna x4 | `ACCURATE_MULTIPLICATION_MMJ` esta activo para este juego (por defecto); no se comparo con el interruptor apagado |

Pendiente: comparar con cada interruptor apagado y encendido en juegos de las listas (Mario Kart 7, Star Fox 64, etc.).

## Procedencia (commits originales de MMJ)

GitHub permite un solo repositorio padre por fork (aqui, AzaharPlus), asi que la relacion con Citra MMJ
(https://github.com/weihuoya/citra) se documenta con enlaces permanentes. Cada fila es el primer commit de MMJ donde
aparece por primera vez el nombre del ajuste (verificado con `git log -S<nombre>` sobre el historial completo, 8715
commits, HEAD `302b0c3`). Los mensajes de MMJ son muy breves y un commit puede agrupar otros cambios.

| Ajuste de MMJ | Commit | Autor | Fecha | Mensaje original |
|---|---|---|---|---|
| `skip_slow_draw` | e85a43cf92e446b4ac4af99a22b53655ea12b92e | weihuoya | 2020-07-06 | android project |
| `disable_clip_coef` | 0467d74465a73aba30b8ab712243f9f14b7bcb2d | weihuoya | 2020-09-21 | minor fx |
| `y2r_perform_hack` | 965594b140bf6c5c3f5caf6a674d5d3a8a44f5a7 | weihuoya | 2020-12-25 | compress shader cache |
| `use_linear_filter` | 21c231fbc2e1d1523a250330b2543b3c96c0875f | weihuoya | 2021-01-18 | use linear filter |
| `skip_texture_copy` | 3c93fb489497dd49b58afa5187e735609b149a53 | weihuoya | 2022-01-20 | skip texture copy |
| `config-games.ini` (creacion) | 79564b5f34fec812c6750b7165099023f83086a7 | weihuoya | 2022-04-19 | fx |
| version usada como fuente | 302b0c3bc4312bc96489c11584a97664894b343f | zhangwei | 2025-11-14 | cache |

URL de cada commit: `https://github.com/weihuoya/citra/commit/<hash>`.
Historial local: `git remote add mmj https://github.com/weihuoya/citra.git && git fetch mmj`.

## Otros cambios de este fork (no vienen de MMJ)

- **Resolucion interna limitada a 8x.** `Settings::values.resolution_factor` pasa de 0..18 a 0..8 y se quitan las opciones 9x-18x de Android, Qt y libretro. Las configuraciones antiguas con valores mayores se acotan a 8 solas.
- **Log de hacks activos:** al iniciar cada juego se registra una linea `Per-game hacks for <ID>: ...` con el estado de cada hack.
- **`-PappIdSuffix=`** en Gradle: anade un sufijo al ID de aplicacion para instalar este fork junto a otras versiones.
