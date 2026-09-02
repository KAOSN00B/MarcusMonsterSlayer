# Architecture

This document describes the runtime structure of Marcus Monster Slayer for
reviewers and new contributors. It reflects the C++ in `Source/MarcusMonsterSlayer/`.

## Module and dependencies

Single runtime module `MarcusMonsterSlayer`. Public dependencies
(`MarcusMonsterSlayer.Build.cs`):

`Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `Paper2D`,
`PaperZD`, `GameplayCameras`.

Plugins (`MarcusMonsterSlayer.uproject`): `PaperZD` (required, runtime),
`ModelingToolsEditorMode` (editor only), `VisualStudioTools` (disabled).

## Boot flow

```
GameInstance (UMarcusMonsterHunterGameInstance)  -- persists across level loads
        |
GameMode  (BP_MarcusMonsterHunterGameMode)       -- set in DefaultEngine.ini
        |  default pawn
        v
AMarcus (APaperZDCharacter)
        |-- USpringArmComponent + UCameraComponent   (side-on framing)
        |-- UBoxComponent  AttackCollisionBox         (enabled only during the attack anim)
        |-- Enhanced Input: IA_Move / IA_Jump / IA_Attack / IA_Pause
        |-- UPlayerHUD widget   (HP bar, currency, level name)
        \-- UPauseMenu widget
```

`DefaultEngine.ini` wires `GameDefaultMap=/Game/Levels/MainMenu`,
`GlobalDefaultGameMode`, and `GameInstanceClass`.

## Gameplay classes

### Player - `AMarcus : APaperZDCharacter`
- Movement/attack gated by independent flags: `IsAlive`, `IsActive`, `CanMove`,
  `CanAttack`, `IsStunned`.
- Combat: `Attack()` plays a PaperZD animation override; `EnableAttackCollisionBox()`
  (BlueprintCallable, driven by an anim notify) opens the damage window;
  `AttackCollisionBoxBeginOverlap()` applies `AttackDamage` (default 2) and a
  push-back impulse to overlapped `AEnemy` / `AFlyingEnemy`.
- Damage in: `TakeDamage(int, float stun)` - **custom non-virtual signature**, not
  `AActor::TakeDamage`. Applies stun via `StunTimer`, routes death to `PlayerDeath()`
  -> `GameOverTimer` -> `GameInstance`.
- `PlayerLastGroundedLocation` is sampled while grounded and used by hazards to
  respawn/teleport the player.
- Upgrades: `UnlockDoubleJump()` flips state mirrored in the GameInstance.
- Pause: `PauseGame()` / `UnPauseGame()` (BlueprintCallable) toggle the pause menu
  and `UGameplayStatics` pause state.

### Enemies - `AEnemy : APaperZDCharacter`
- Perception: `PlayerDetectorSphere` overlap sets/clears `FollowTarget` (`AMarcus*`).
- Steering: `ShouldMoveToTarget()` / `UpdateDirection()` walk toward the player
  until `StoppingDistance`, then `AttackPlayer()` on `AttackCooldownTimer`.
- Animation: `StateMachineName` (default `"OgreStateMachine"`) **must match the
  name of the state machine in each enemy's PaperZD AnimBP**; `JumpTakeHit` /
  `JumpDie` nodes are jumped into by name.
- Health: `UpdateHitPoints()` pushes to a `UEnemyHealthBar` widget component and a
  debug `UTextRenderComponent`. `TakeDamage(dmg, stun, pushback)` -> stun, optional
  `ApplyPushBack()`, death anim override -> actor destroyed on override-end.
- Ogre and Skeleton Warrior are Blueprint subclasses that only differ in stats,
  sprites, and AnimBP.

### Flying enemy - `AFlyingEnemy : AActor`
- Not a character: a `UPaperFlipbookComponent` + `UBoxComponent` contact volume.
- Deterministic patrol between `PatrolStart` and `PatrolStart + PatrolOffset`,
  `PatrolSpeed` round-trips/second; optional sprite flip toward travel direction.
- `OnContactBegin` deals `ContactDamage` + stun to Marcus. `TakeHit(int)` from the
  player's sword; at 0 HP plays `DeathAnim` (scaled by `DeathAnimScale`) + sound,
  then destroys itself.

### World / props
| Class | Base | Role |
|---|---|---|
| `ACheckpoint` | `AActor` | Trigger box + candle flipbook (inactive/active). On overlap: `GameInstance->SetCheckpoint(level, location)`, swaps to lit anim, one-shot. |
| `ACollectableItem` | `AActor` | Capsule + flipbook. `CollectableType` in {Money, HealthPotion, DoubleJumpUpgrade}; on overlap calls `AMarcus::CollectItem(type, amount)`. |
| `ALevelExit` | `AActor` | Door flipbook + `TSoftObjectPtr<UWorld> NextLevel`. On overlap: play sound, wait `WaitTimeInSeconds`, then travel. |
| `AMyTriggerBox` | `ATriggerBox` | Damaging hazard. Optional timed active/inactive cycle (`ActiveDuration` / `InactiveDuration` / `StartDelay`), optional delayed teleport of the player back to `PlayerLastGroundedLocation`, optional despawn. |
| `AWorldParalaxManager` | `AActor` | Holds `TArray<FParallaxLayer>`. Each layer references a placed `APaperSpriteActor`, attaches it to the player camera at `BeginPlay`, optionally fits it to the view, and drives a dynamic material instance (`M_ParalaxSprite`, params `CameraPosX` and `Paralax Multiplyer`) each tick. |

### UI - `UUserWidget`
- `UPlayerHUD` - `HPBar` (ProgressBar), `CurrencyText`, `HPText`, `LevelText` via `BindWidget`.
- `UPauseMenu` - `ResumeButton` / `QuitButton`; binds clicks in `NativeConstruct`.
- `UEnemyHealthBar` - single `HealthBar` progress bar; `SetHealth(cur, max)`.

## State & persistence model

`UMarcusMonsterHunterGameInstance` is the only cross-level state:

| Field | Purpose |
|---|---|
| `PlayerHP`, `PlayerMaxHP`, `HasInitializedHP` | HP carried between levels; seeded once |
| `CollectedMoneyCount` | Currency |
| `IsDoubleJumpUnlocked` | Upgrade flag |
| `StartingLevel`, `NextLevel` | Level routing (soft world pointers) |
| `HasCheckpoint`, `CheckpointLevelName`, `CheckpointLocation` | Respawn target |

Key methods: `ChangeLevel()`, `SetCheckpoint()` / `RespawnAtCheckpoint()`,
`TriggerGameOverScreen()`, `RestartGame()` (BlueprintCallable). There is **no
disk save** - progress lives only for the session.

## Configuration of note

- `Config/DefaultGameUserSettings.ini` - ships windowed 1280x720
  (`FullscreenMode=2`). Only seeds a fresh install; a user's saved
  `GameUserSettings.ini` overrides it.
- `Config/DefaultEngine.ini` - `[SystemSettings] r.SetRes=1280x720w`;
  `CoreRedirects` maps the legacy `Ogre` class to `Enemy`;
  `ActiveGameNameRedirects` maps the `TP_Blank` template name.
- `Config/DefaultGame.ini` - `bSkipEditorContent=True`, Shipping packaging.

## Audit notes / known limitations

- `AMarcus::TakeDamage` / `AEnemy::TakeDamage` deliberately shadow
  `AActor::TakeDamage` with project-specific signatures; they are **not** the
  engine damage pipeline. No `UGameplayStatics::ApplyDamage` is used.
- Wide public surface: most gameplay fields are `UPROPERTY(VisibleAnywhere,
  BlueprintReadWrite)` for live tuning. Encapsulation is intentionally low.
- Header coupling: `Marcus.h` includes `PlayerHUD.h`, `PauseMenu.h`,
  `MarcusMonsterHunterGameInstance.h`, `CollectableItem.h`, pulling UI/UMG into
  most translation units. Forward declarations would cut compile time.
- Enemy AnimBP state-machine names are matched by string (`StateMachineName`);
  a rename in the AnimBP silently breaks hit/death transitions.
- Identifier typos are load-bearing: material params `Paralax Multiplyer`,
  class/dir `Paralax`. Renaming requires touching assets, not just code.
- No automated tests and no in-engine functional tests yet (see roadmap).
- Flying enemy patrol is frame-rate-independent (time-parameterised) but does not
  sweep for blocking geometry.
