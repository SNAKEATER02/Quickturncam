# Quick Turn Camera (GTA SA Android / AML)

A 64-bit ARM native AML plugin for GTA San Andreas Android. It speeds up the vehicle-follow camera's turn response while the player is actively steering left or right. When steering input is neutral, the original camera speed arguments are passed through unchanged.

## Features

- Targets `arm64-v8a` only.
- Hooks `CCam::Process_FollowCar_SA` through the AML hook interface and the game symbol supplied by the bundled plugin SDK.
- Uses `CPad::GetSteeringLeftRight`, so the check follows the game's vehicle steering input, including touch controls.
- Multiplier slider in the game's Settings > Controls tab: 1.0x–5.0x, 0.1x steps, default 2.0x.
- Saves the selected multiplier in the AML config.

## Build

Requires Android NDK with `ndk-build`.

1. Put the path to your Android NDK directory in a text file named `NDKPath.txt` in this folder.
2. Run `./build.sh` on Linux/macOS, or `build.ps1` on Windows.
3. The ARM64 library will be created at `libs/arm64-v8a/libQuickTurnCamera.so`.

The project is configured for `arm64-v8a` and Android API 24+.

## Install

Copy `libs/arm64-v8a/libQuickTurnCamera.so` into the AML mods directory for your GTA SA installation, then launch the game and adjust **Settings > Controls > Camera turn speed**.

## Compatibility note

The hook uses the symbol `CCam::Process_FollowCar_SA` resolved from the game library at runtime by the bundled SDK. The supplied `libGTASA.so` was confirmed to be ELF64 AArch64 and to export this symbol at file-relative ELF address `0x49FD38`. Test in-game before relying on it; camera tuning can vary with game build and other camera mods. Back up your game files before testing.

## Credits and licenses

The project retains the upstream AML Android plugin template and its bundled plugin SDK. See `LICENSE` and `licenses/` for the applicable license texts.
