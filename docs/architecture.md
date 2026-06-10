# Architecture

The repository is a small ESP-IDF app component with portable code in `lib/`.

## Layers

- `src/main.cpp`: ESP-IDF entrypoint. Initializes logging, prints chip info and starts `App`.
- `src/app/`: application shell, events and state.
- `src/hardware/`: board-specific constants and future hardware abstractions.
- `src/services/`: future app-facing subsystem orchestration.
- `src/ui/`: future display/UI integration.
- `lib/common/`: portable code with no ESP-IDF dependency.

## Rules

- `BoardPins.hpp` owns all physical pin numbers.
- Hardware code stays under `src/hardware`.
- Portable code stays under `lib/`.
- Feature flags exist for subsystems, but real drivers are not implemented yet.
- ES8311, BMI270, LVGL, BLE and USB expose explicit hooks. When the real bring-up sequence is missing, the hook returns `ESP_ERR_NOT_SUPPORTED`.
- WiFi exposes an opt-in ESP-IDF station service for init, scan, connect and connection wait.
- LVGL and USB are prepared with flags only; dependencies are not added yet.
