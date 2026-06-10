# Display

- Panel: ST7789V2
- Size: 240 x 135
- Backlight: G38
- Reset: G33
- DC/RS: G34
- MOSI/DAT: G35
- SCK: G36
- CS: G37

Expected sequence:

1. Configure backlight GPIO.
2. Initialize SPI host.
3. Send the minimal ST7789 command sequence.
4. Enable display and draw pixels.

Status: direct ESP-IDF SPI initialization and pixel drawing are prepared. Text rendering is a placeholder; rotation and offsets need hardware validation. `esp_lcd` remains a candidate for a future optional display backend after local compiler/build stability is verified.

Smoke test:

```bash
pio run -e cardputer_adv_display_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
