# Vaulkeeper

Gestionnaire de stockage natif pour Windows. Analyse ce qui prend de la place sur ton PC, trouve les doublons, desinstalle proprement les applications, automatise la maintenance. Ecrit en C++23 avec Qt 6.

Disponible en cinq langues : anglais, francais, espagnol, allemand, italien.

## Installation

Telecharge `VaulkeeperSetup.exe` depuis la derniere release :

https://github.com/Matteo-CB/vaulkeeper/releases

Double clique dessus, accepte la licence, clique Installer. Au premier lancement, choisis ta langue.

Windows SmartScreen affichera un avertissement bleu parce que l'installeur n'est pas signe (certificat EV a venir). Clique "Informations complementaires" puis "Executer quand meme".

## Ce que Vaulkeeper fait

* Lecture directe de la MFT NTFS pour un scan sous 15 secondes sur 2 To de disque.
* Treemap, sunburst et tableau trie pour visualiser l'occupation.
* Detection de doublons exacts via xxHash3, doublons approximatifs prevus pour les photos.
* Desinstallation qui nettoie les fichiers et les cles de registre que l'uninstaller officiel laisse.
* Quarantaine de 30 jours avec restore en un clic avant suppression definitive.
* Lecture SMART des disques physiques pour predire les pannes.
* Moteur de regles YAML pour automatiser le nettoyage sur cron, seuil d'espace, insertion de disque.
* Integration Explorer (clic droit, "Analyser avec Vaulkeeper").
* CLI `vk` scriptable avec sortie JSON.
* Agent elevee separe pour les operations admin, jamais actif en arriere plan par defaut.

## Trois binaires

| Fichier | Role |
|---------|------|
| `vaulkeeper.exe` | Interface graphique Qt QML |
| `vk.exe` | Ligne de commande, exit codes normalises, sortie JSON |
| `VaulkeeperAgent.exe` | Helper elevee, demarre a la demande via UAC |

## Exemples CLI

```
vk scan
vk duplicates C:\Users\me\Downloads
vk health
vk quarantine list
vk rules run cleanup.downloads --dry-run
vk report --json
```

## Principes

* Securite d'abord : aucune suppression immediate, tout passe par la quarantaine.
* Vitesse : lecture MFT, hashing SIMD, cache incremental, UI a 120 FPS pendant le scan.
* Zero telemetrie par defaut. Aucun fichier ne quitte la machine sans opt-in explicite.
* Aucun emoji, aucun dash long, aucun texte marketing dans l'app. Les messages disent ce qui s'est passe, ce que ca implique, et ce que tu peux faire.

## Configuration systeme

* Windows 10 22H2 ou Windows 11
* x64 uniquement
* 8 Go de RAM recommandes (fonctionne avec 4 Go)
* 200 Mo d'espace disque

## Compiler depuis les sources

Pre-requis :

* Visual Studio 2022 Build Tools (MSVC v143)
* CMake 3.28+, Ninja
* vcpkg (`VCPKG_ROOT` defini)
* WiX Toolset 3.14+ pour l'installeur

```
powershell -ExecutionPolicy Bypass -File scripts\bootstrap.ps1 -Preset msvc-relwithdebinfo
powershell -ExecutionPolicy Bypass -File scripts\build-installer.ps1
```

La premiere fois, vcpkg compile Qt 6, OpenSSL, TBB, SQLite, etc. Compte 1 a 2 heures. Les builds suivants sont incrementaux.

Les binaires sortent dans `build/<preset>/bin/`. L'installeur `VaulkeeperSetup.exe` sort dans `build/msvc-release-installer/installer/`.

## Architecture

```
src/core           moteur portable (scan, doublons, quarantine, journal, regles, cleaners)
src/platform       adaptateurs Windows (MFT, WMI, ETW, SMART, registry, services)
src/app            interface Qt QML
src/cli            vk
src/agent          VaulkeeperAgent
src/updater        vk-updater signe
src/shellext       extension Explorer (COM IShellExtInit)
src/installer      manifest WiX MSI + bundle Burn
tests              unit, integration, fuzz, perf
packaging          winget, chocolatey, scoop
```

## Licence

Code source proprietaire. Les dependances tierces sont listees dans l'about de l'app. Qt est lie dynamiquement sous LGPL.

## Support

Ouvre une issue sur ce repo avec les etapes pour reproduire, le contenu de `%LOCALAPPDATA%\Vaulkeeper\logs\vaulkeeper.log` et la version (menu About).
