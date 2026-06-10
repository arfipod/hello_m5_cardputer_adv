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
4. Enable display and draw pixels, shapes and basic text.

Status: direct ESP-IDF SPI initialization, pixel drawing, filled/outlined rectangles, lines and a compact 5x7 text renderer are prepared. The driver exposes the panel as 240 x 135 landscape and applies the Cardputer/Cardputer-Adv ST7789 address-window offsets X=40/Y=53. `esp_lcd` remains a candidate for a future optional display backend after local compiler/build stability is verified.

Smoke test:

```bash
pio run -e cardputer_adv_display_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
