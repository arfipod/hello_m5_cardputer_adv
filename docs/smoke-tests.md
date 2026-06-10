# Smoke Tests

Smoke tests are separate firmware builds selected by PlatformIO environments. Each mode validates one subsystem and avoids initializing the rest of the board.

Use COM5 locally:

```bash
pio run -e cardputer_adv_i2c_scan -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```

## Environments

| Environment | Mode | Status |
| --- | --- | --- |
| `cardputer_adv` | `boot_info` | Implemented. Prints chip, flash, heap and reset reason. |
| `cardputer_adv_i2c_scan` | `i2c_scan` | Implemented. Reports all detected addresses and expected devices. |
| `cardputer_adv_keyboard_test` | `keyboard_test` | Partial. TCA8418 register path and placeholder layout need hardware validation. |
| `cardputer_adv_display_test` | `display_test` | Partial. Uses direct SPI ST7789 commands and draws bars/lines; text drawing is still a placeholder. |
| `cardputer_adv_audio_test` | `audio_test` | Stubbed for codec. ES8311 init returns `ESP_ERR_NOT_SUPPORTED`; tone is skipped until codec registers are implemented. |
| `cardputer_adv_sd_test` | `sd_test` | Prepared. Mounts and lists `/sdcard`; write test is guarded by `APP_SMOKE_SD_WRITE=1`. |
| `cardputer_adv_imu_test` | `imu_test` | Stubbed for BMI270 init. Probe is implemented, firmware/config upload is pending. |
| `cardputer_adv_battery_test` | `battery_test` | Partial. ADC raw read works; voltage conversion needs divider/calibration values. |
| `cardputer_adv_ir_test` | `ir_test` | Prepared. Uses RMT copy encoder to send a short pattern. |
| `cardputer_adv_lvgl_demo` | `lvgl_demo` | Pending. Builds with `APP_ENABLE_LVGL=1`, but LVGL dependency and screen are not added yet. |

## Validation Matrix

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

CI builds the default firmware, fallback firmware and selected smoke images. CI never flashes hardware.
