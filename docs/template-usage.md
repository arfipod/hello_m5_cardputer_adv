# Template Usage

This repository is a reusable base for future Cardputer-Adv projects, not a finished application.

## Where Code Belongs

- Application behavior: `src/app/`
- Manual diagnostics: `src/apps/smoke_tests/`
- Hardware-specific code: `src/hardware/`
- Optional app-facing services: `src/services/`
- UI integration: `src/ui/`
- Portable host-testable logic: `lib/`

Pin numbers belong only in `src/hardware/BoardPins.hpp`.

## Feature Flags

Subsystems use `APP_ENABLE_*` flags. Smoke applications use `APP_MODE_*` flags. The default firmware is `boot_info`.

Do not initialize every subsystem at boot. Create the HAL object you need, call `init()` explicitly, and keep project-specific orchestration in the app layer.

## Dependencies

The template currently relies on ESP-IDF built-ins for GPIO, I2C, SPI, I2S, ADC, RMT and FATFS/SDSPI.

LVGL, TinyUSB, ES8311 helper components and BMI270 vendor drivers are intentionally not added yet.
