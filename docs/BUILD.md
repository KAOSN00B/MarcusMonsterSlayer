# Building & packaging

## Prerequisites

| Requirement | Version |
|---|---|
| Unreal Engine | 5.8 (from the Epic Games Launcher or source) |
| Visual Studio | 2022, "Game development with C++" workload + MSVC v143 |
| Windows SDK | 10.0.22621 or newer |
| Git | 2.40+ with **Git LFS** installed (`git lfs install`) |
| PaperZD plugin | Installed for UE 5.8 (Fab / marketplace) |

## Clone

```bash
git clone https://github.com/KAOSN00B/marcus-monster-slayer.git
cd marcus-monster-slayer
git lfs pull          # fetch .uasset / .umap / audio / textures
```

## Generate project files & build the editor

1. Right-click `MarcusMonsterSlayer.uproject` -> **Generate Visual Studio project files**.
2. Open `MarcusMonsterSlayer.sln`.
3. Set configuration to **Development Editor**, platform **Win64**.
4. Build. First build compiles shaders and may take a while.
5. Run - the editor opens on `Content/Levels/Level_1` (editor startup map).
   Open `Content/Levels/MainMenu` to test the full flow.

## Command-line build

```bash
# from the engine's Engine/Build/BatchFiles directory
Build.bat MarcusMonsterSlayerEditor Win64 Development -project="<abs>\MarcusMonsterSlayer.uproject"
```

## Packaging a Windows build

Editor: **Platforms -> Windows -> Package Project**, or:

```bash
RunUAT.bat BuildCookRun ^
  -project="<abs>\MarcusMonsterSlayer.uproject" ^
  -noP4 -platform=Win64 -clientconfig=Shipping ^
  -cook -build -stage -pak -archive -archivedirectory="<abs>\dist"
```

Output runs windowed at 1280x720 (`Config/DefaultGameUserSettings.ini`).

## Troubleshooting

### Cook error: "Content is missing from cook ... marked NeverCook"
A cookable asset hard-references editor-only engine content
(`/Engine/Editor*`). Fix the reference, not the flag. Seen in this project:

- **Anim notifies** pointing at `/Engine/EditorSounds/...` - repoint the
  PaperZD `PlaySound` notify to an asset under `/Game/`.
- **Landmass plugin** brush materials referencing `/Engine/EditorMaterials/...` -
  disable Landmass + Water if unused, or add
  `+DirectoriesToNeverCook=(Path="/Landmass/Landscape/BlueprintBrushes/Materials")`
  to `Config/DefaultGame.ini`.

### Packaged build ignores the windowed default
`DefaultGameUserSettings.ini` only seeds a *fresh* install. Delete the stale
per-user file and relaunch:
`%LOCALAPPDATA%\MarcusMonsterSlayer\Saved\Config\Windows\GameUserSettings.ini`.
Or run the exe with `-windowed -ResX=1280 -ResY=720`.

### LFS files show up as text pointers
`git lfs install` was not run before cloning. Run it, then `git lfs pull`.
