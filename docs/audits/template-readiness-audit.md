# Template Readiness Audit

Date: 2026-06-10

## Fully Implemented

- PlatformIO ESP-IDF project structure.
- Native Unity test environment.
- GitHub Actions compile/test workflow.
- `BoardPins.hpp` as the single source of pin mapping.
- Generic `app_main` and build-flag mode selector.
- Boot info smoke mode.
- Shared I2C bus wrapper with mutex, register helpers and scan.
- I2S transport wrapper for 16-bit sample buffers.
- RMT-based IR raw transmit wrapper.
- ADC raw battery read path and approximate calibrated battery millivolts.

## Prepared But Stubbed Or Partial

- TCA8418 keyboard: register skeleton and placeholder logical map exist; matrix mapping needs hardware validation.
- ST7789 display: direct ESP-IDF SPI init, pixel drawing, simple shapes and compact 5x7 text exist; geometry tuning needs hardware validation.
- ES8311 codec: probe path exists; register initialization returns `ESP_ERR_NOT_SUPPORTED`.
- BMI270 IMU: probe path exists; firmware/config upload returns `ESP_ERR_NOT_SUPPORTED`.
- microSD: SDSPI mount/list path exists; write test is opt-in.
- LVGL: feature flag and demo environment exist; no LVGL dependency is added yet.
- USB, WiFi and BLE services: service shells exist; no class/device behavior is implemented.

## Intentionally Optional

- LVGL for richer UI.
- TinyUSB/USB CDC/MIDI/HID experiments.
- WiFi/BLE helpers.
- Third-party ES8311, BMI270 or graphics components.
- SD write tests, gated by `APP_SMOKE_SD_WRITE=1`.

## Dependency Rationale

- ESP-IDF built-ins are used for GPIO, I2C, SPI, I2S, ADC, RMT and FATFS/SDSPI.
- No Arduino framework or M5Cardputer Arduino library is used.
- No external component-manager dependencies are added yet because ES8311, BMI270 and LVGL choices still need hardware/license/version verification.
- `sdkconfig.defaults` uses `CONFIG_COMPILER_OPTIMIZATION_NONE=y` because the local ESP-IDF 5.5 toolchain hit an internal compiler error while compiling unused ESP-IDF display code at the default optimization level.

## Known Hardware Risks

- TCA8418 row/column mapping may differ from the placeholder layout.
- ES8311 codec address and register sequence must be validated on the Cardputer-Adv.
- BMI270 may require a vendor firmware blob and a specific init sequence.
- ST7789V2 offsets, rotation and color order may require adjustment.
- `esp_lcd` is not in the base build because the local fallback environment hit a compiler crash while compiling unused RGB panel code; it should be reintroduced only behind a verified optional backend.
- Display and microSD use separate SPI hosts in the template; combined usage needs board validation.
- Battery voltage conversion uses ESP-IDF ADC calibration and the Cardputer/Cardputer-Adv 2.0 voltage ratio; percentage/charging state still need policy-level validation.
- IR output should be checked with a receiver before relying on timing.

## Build And Test Commands

```bash
pio test -e native
pio run -e cardputer_adv
pio run -e cardputer_adv_devkit_fallback
pio run -e cardputer_adv_i2c_scan
pio run -e cardputer_adv_keyboard_test
pio run -e cardputer_adv_display_test
pio run -e cardputer_adv_audio_test
pio run -e cardputer_adv_sd_test
pio run -e cardputer_adv_imu_test
pio run -e cardputer_adv_battery_test
pio run -e cardputer_adv_ir_test
pio run -e cardputer_adv_lvgl_demo
```

## Recommended Use

Start from `boot_info` and `i2c_scan`, then validate one peripheral at a time. Only after the relevant smoke test passes should application logic depend on that subsystem.

## Go/No-Go

Go for use as a reusable ESP-IDF/PlatformIO template and learning base.

No-go for assuming all Cardputer-Adv peripherals are production-ready. Keyboard, display tuning, ES8311, BMI270, audio path and battery calibration still need connected-device validation.
