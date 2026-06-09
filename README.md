# M5 Cardputer-Adv ESP-IDF Template

Reusable base template for M5 Cardputer-Adv development with VS Code,
PlatformIO, ESP-IDF and C++17.

This repository is intentionally application-neutral. It provides a buildable
project skeleton, pin map, documentation, CI, and portable host-testable common
code. Real display, keyboard, audio, IMU, SD, battery and IR drivers are planned
but not implemented in this skeleton step.

## What Is Implemented

- PlatformIO ESP-IDF environments for `m5stack-stamps3` and `esp32-s3-devkitc-1`.
- Native test environment for portable code.
- Minimal `app_main()` that configures logging, prints chip information and starts `App`.
- Generic `App` loop with subsystem feature flag reporting.
- `src/hardware/BoardPins.hpp` as the single source of truth for pin mapping.
- `lib/common` with small portable helpers and tests.
- VS Code recommendations.
- GitHub Actions CI.
- Documentation for setup, flashing, architecture, pinout, testing and troubleshooting.

## What Is Planned

- ESP-IDF-native peripheral drivers.
- Shared I2C bus abstraction for TCA8418, ES8311 and BMI270.
- Display abstraction for ST7789V2.
- Audio abstraction for ES8311 and I2S.
- microSD, battery, IR and IMU services.
- Optional LVGL and USB/TinyUSB integration.
- Hardware smoke examples for manual validation.

## Build

```sh
pio run -e cardputer_adv
pio run -e cardputer_adv_devkit_fallback
```

## Test

```sh
pio test -e native
```

## Upload And Monitor

```sh
pio run -e cardputer_adv -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```

## Layout

```text
src/main.cpp        ESP-IDF entrypoint
src/app/            generic app shell, events and state
src/hardware/       board-specific pin map and future hardware abstractions
src/services/       future app-facing subsystem services
src/ui/             future UI integration
lib/common/         portable code shared by firmware and host tests
test/               PlatformIO native tests
docs/               usage, architecture and hardware notes
```
