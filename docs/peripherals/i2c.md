# I2C

The Cardputer-Adv shared I2C bus is on G8/G9 and is used by:

- TCA8418 keyboard at `0x34`
- BMI270 IMU at `0x69`
- ES8311 codec candidate address `0x18`
- Grove/EXT devices if attached to the same lines

The `I2CBus` wrapper owns ESP-IDF I2C master initialization, a FreeRTOS mutex, register helpers and a scan helper.

Expected sequence:

1. Construct `hardware::I2CBus`.
2. Call `init()`.
3. Share the bus reference with device drivers.
4. Use driver APIs instead of hardcoding transactions in app code.

Smoke test:

```bash
pio run -e cardputer_adv_i2c_scan -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
