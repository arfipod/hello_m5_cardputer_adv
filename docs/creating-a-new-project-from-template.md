# Creating A New Project From This Template

1. Create a new repository from this template.
2. Update README names, CI artifact names and project-specific docs.
3. Keep `BoardPins.hpp` unchanged unless the target board changes.
4. Run baseline validation:

```bash
pio test -e native
pio run -e cardputer_adv
pio run -e cardputer_adv_i2c_scan
```

5. Flash `cardputer_adv_i2c_scan` first and verify the shared I2C bus.
6. Bring up one subsystem at a time with the smoke-test environments.
7. Move product behavior into `src/app/` only after the relevant smoke test passes.
8. Add native tests for portable logic before coupling it to hardware.

Recommended order:

1. `boot_info`
2. `i2c_scan`
3. `battery_test`
4. `display_test`
5. `keyboard_test`
6. `sd_test`
7. `imu_test`
8. `audio_test`
9. `ir_test`
10. optional LVGL, USB, WiFi and BLE
