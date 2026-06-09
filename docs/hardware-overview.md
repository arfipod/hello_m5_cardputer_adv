# Hardware Overview

Target device: M5 Cardputer-Adv.

- Core: Stamp-S3A / ESP32-S3FN8
- Flash: 8 MB
- Display: ST7789V2, 240 x 135
- Keyboard: TCA8418 over I2C
- Audio codec: ES8311 over I2C, audio over I2S
- IMU: BMI270 over I2C
- Storage: microSD over SPI
- Battery monitor: ADC on G10
- IR TX: G44
- Grove and EXT expansion pins

This skeleton only records the hardware map and project architecture. Real
peripheral drivers are intentionally planned for later steps.
