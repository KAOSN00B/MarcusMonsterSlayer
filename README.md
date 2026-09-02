# Marcus Monster Slayer

A 2D gothic action-platformer built with **Unreal Engine 5.8** (Paper2D + PaperZD).
Play as Marcus, a monster slayer cutting through a cursed cathedral of ogres,
skeleton warriors, and flying skulls.

**Status:** playable vertical slice, in active development.

[![CI](https://github.com/KAOSN00B/marcus-monster-slayer/actions/workflows/ci.yml/badge.svg)](https://github.com/KAOSN00B/marcus-monster-slayer/actions/workflows/ci.yml)
[![Engine](https://img.shields.io/badge/Unreal%20Engine-5.8-blue)](https://www.unrealengine.com)
[![License](https://img.shields.io/badge/code-MIT-green)](LICENSE)

Play it on itch.io: <https://kaosn00b.itch.io/marcus-monster-hunter>

## Contents

- [Overview](#overview)
- [Gameplay](#gameplay)
- [Controls](#controls)
- [Getting the game](#getting-the-game)
- [Building from source](#building-from-source)
- [Repository layout](#repository-layout)
- [Architecture](#architecture)
- [Contributing](#contributing)
- [Licensing & third-party assets](#licensing--third-party-assets)

## Overview

| | |
|---|---|
| Engine | Unreal Engine 5.8 |
| Language | C++ (gameplay) + Blueprint (composition, UI, tuning) |
| Rendering | Paper2D sprites / flipbooks |
| Animation | PaperZD (state machines + animation overrides) |
| Input | Enhanced Input |
| Target platform | Windows (Win64, Shipping) — windowed 1280x720 by default |
| Primary module | `MarcusMonsterSlayer` (Runtime) |

## Gameplay

- Run-and-slash platforming across hand-authored levels (`MainMenu -> Level_1 -> Level_2`).
- Melee combat vs. three enemy archetypes, each with its own animation set and health bar.
- Checkpoints, collectables (money / health potion / double-jump upgrade), and
  level-to-level transitions carried by a persistent `GameInstance`.
- Environmental hazards, parallax backgrounds, pause menu, and a player HUD.

### Enemies

| Enemy | Class | Behaviour |
|---|---|---|
| Ogre | `AEnemy` (Blueprint subclass) | Detects the player via a sphere, walks to melee range, attacks on cooldown |
| Skeleton Warrior | `AEnemy` (Blueprint subclass) | Same core AI, different stats / animation set |
| Flying Skull | `AFlyingEnemy` | Ping-pong patrol, contact damage, one hit to kill |

## Controls

| Action | Keyboard |
|---|---|
| Move | A / D |
| Jump (double-jump once unlocked) | Space |
| Attack | Left Mouse Button |
| Pause | Esc |

Bindings live in `Content/InputActions/` (`IA_Move`, `IA_Jump`, `IA_Attack`,
`IA_Pause`, `InputMappingContext_MarcusMonsterHunter`).

## Getting the game

Download the latest Windows build from
[Releases](../../releases) or [itch.io](https://kaosn00b.itch.io/marcus-monster-hunter),
unzip anywhere, and run `MarcusMonsterSlayer.exe`. It launches in a 1280x720
window; change the mode in the options menu or pass `-fullscreen`.

## Building from source

See [`docs/BUILD.md`](docs/BUILD.md) for the full walkthrough. Short version:

```bash
git clone https://github.com/KAOSN00B/marcus-monster-slayer.git
cd marcus-monster-slayer
git lfs pull
```

1. Install Unreal Engine 5.8 and the PaperZD plugin.
2. Right-click `MarcusMonsterSlayer.uproject` -> **Generate Visual Studio project files**.
3. Open `MarcusMonsterSlayer.sln`, build **Development Editor | Win64**.
4. Launch the `.uproject`; open `Content/Levels/MainMenu` and press Play.

## Repository layout

| Path | Contents |
|---|---|
| `Source/MarcusMonsterSlayer/` | All C++ gameplay classes (see [Architecture](#architecture)) |
| `Source/*.Target.cs` | Game and Editor build targets |
| `Content/Blueprints/` | Blueprint subclasses: `PlayerMarcus`, `Ogre`, `SkeletonWarrior`, `FlyingSkull`, `Items`, `Other` (game mode / instance) |
| `Content/Levels/` | `MainMenu`, `Level_1`, `Level_2`; `TestLevel` is a scratch map (not shipped) |
| `Content/InputActions/` | Enhanced Input actions + mapping context |
| `Content/Assets/ansimuz/` | Imported art / audio (third-party - see notices) |
| `Config/` | `DefaultEngine.ini`, `DefaultGame.ini`, `DefaultInput.ini`, `DefaultGameUserSettings.ini` |
| `docs/` | Architecture and build documentation |

Generated folders (`Binaries/`, `Build/`, `Intermediate/`, `Saved/`,
`DerivedDataCache/`) are not tracked.

## Architecture

Full breakdown in [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md), including the
run-time object graph, the state/persistence model, and known limitations.

## Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for branch naming, commit conventions,
the C++ style (`.clang-format`, Unreal coding standard), and the review
checklist. All participants are held to the [Code of Conduct](CODE_OF_CONDUCT.md).

## Licensing & third-party assets

- **Code** (`Source/`) is released under the [MIT License](LICENSE).
- **Art, audio, and fonts** under `Content/Assets/` are third-party and retain
  their original licenses. Sprite and audio art is by
  [Luis Zuno (ansimuz)](https://ansimuz.itch.io/). Provenance and license terms
  are tracked in [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md). Do not assume
  the code license applies to any asset.
