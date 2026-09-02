# Contributing

## Workflow

1. Branch from `main`: `feature/<short-name>`, `fix/<short-name>`, or `docs/<short-name>`.
2. Keep changes focused; one logical change per PR.
3. Open a PR using the template. CI (formatting + LFS integrity) must pass.
4. At least one review approval before merge. Squash-merge.

## Commit messages

Conventional Commits:

```
feat(enemy): flying skull dive attack
fix(marcus): double jump consumed when walking off a ledge
docs(architecture): document persistence model
chore(ci): cache DDC on the self-hosted runner
```

## C++ style

- Follow the [Unreal Engine Coding Standard](https://docs.unrealengine.com/latest/en-US/epic-cplusplus-coding-standard-for-unreal-engine/).
- Formatting is enforced by `.clang-format` (`clang-format -i Source/**/*.{h,cpp}`).
  CI runs `--dry-run --Werror`.
- Prefixes: `A` actors, `U` UObjects/widgets, `F` structs, `E` enums, `b` for bools
  (existing code uses bare `Is*/Can*` bools - match the file you are editing).
- Prefer forward declarations in headers; include in the `.cpp`.
- `UPROPERTY` new gameplay state as `EditAnywhere` for designers; expose to
  Blueprint only when a Blueprint actually needs it.

## Assets

- New third-party assets: add a row to `THIRD_PARTY_NOTICES.md` (source, author,
  license, URL) in the same PR. No asset without a known license.
- Keep imported source art out of the repo unless its license allows redistribution.
- Anim notifies and material params must reference `/Game/` content only - never
  `/Engine/Editor*` (it breaks packaging).

## Adding an enemy

1. New `AEnemy` Blueprint subclass under `Content/Blueprints/<Name>/`.
2. PaperZD AnimBP whose state machine name matches the subclass `StateMachineName`.
3. Set stats (`MaxHitPoints`, `AttackPower`, `AttackCooldownInSeconds`, `StoppingDistance`).
4. Place in a level; verify the detector sphere, health bar, and hit/death transitions.

## Reporting bugs / requesting features

Use the GitHub issue templates.
