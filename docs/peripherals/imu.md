# IMU

- Device: BMI270
- Bus: shared I2C on G8/G9
- Address: `0x69`

Expected sequence:

1. Initialize `I2CBus`.
2. Read chip ID.
3. Upload/configure BMI270 firmware if required.
4. Configure accelerometer and gyroscope ranges.
5. Poll sample registers or enable interrupts.

Status: probe is prepared. Full initialization is intentionally stubbed because BMI270 firmware and license constraints must be verified.

Smoke test:

```bash
pio run -e cardputer_adv_imu_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
