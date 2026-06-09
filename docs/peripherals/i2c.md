# I2C

The Cardputer-Adv shared I2C bus uses:

- SDA: G8
- SCL: G9

Known onboard devices:

- TCA8418 keyboard, expected `0x34`
- BMI270 IMU, expected `0x69`
- ES8311 codec, candidate `0x18` or `0x19`

The `I2CBus` class owns initialization, scanning, register reads/writes and a
FreeRTOS mutex.
