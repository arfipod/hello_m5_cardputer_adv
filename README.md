# M5 Cardputer-Adv ESP-IDF Template

Reusable base template for M5 Cardputer-Adv development with VS Code, PlatformIO, ESP-IDF and C++17.

This repository is intentionally application-neutral. It provides a buildable project skeleton, a single pin map, hardware abstraction boundaries, smoke-test firmware modes, CI, documentation and portable host-testable common code.

## Implemented

- PlatformIO ESP-IDF environments for `m5stack-stamps3` and `esp32-s3-devkitc-1`.
- Native test environment for portable code.
- Generic `app_main()` and `App` shell.
- Build-flag-selected smoke-test modes.
- `src/hardware/BoardPins.hpp` as the single source of truth for pin mapping.
- HAL scaffolding for I2C, TCA8418, ES8311, I2S, ST7789, BMI270, microSD, battery ADC and IR.
- Optional service shells for LVGL, USB, WiFi and BLE.
- GitHub Actions CI.

## Status

Some paths are real and buildable but still require hardware validation. ES8311 codec init, BMI270 init, LVGL UI and USB/WiFi/BLE behavior are intentionally stubbed until their dependencies and board-specific sequences are verified.

See [docs/audits/template-readiness-audit.md](docs/audits/template-readiness-audit.md).

## Build And Test

```sh
pio test -e native
pio run -e cardputer_adv
pio run -e cardputer_adv_devkit_fallback
```

## Upload And Monitor

```sh
pio run -e cardputer_adv -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```

Smoke tests are selected by environment:

```sh
pio run -e cardputer_adv_i2c_scan -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```

See [docs/smoke-tests.md](docs/smoke-tests.md).

## Layout

```text
src/main.cpp              ESP-IDF entrypoint
src/app/                  generic app shell, events and state
src/apps/smoke_tests/     build-selected hardware smoke modes
src/hardware/             Cardputer-Adv HAL and pin map
src/services/             optional service shells
src/ui/                   UI integration shells
lib/common/               portable code shared by firmware and host tests
test/                     PlatformIO native tests
docs/                     usage, architecture and hardware notes
```
