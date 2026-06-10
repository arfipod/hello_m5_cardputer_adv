# Display: ST7789V2

Pins:

- BL: G38
- RST: G33
- DC/RS: G34
- MOSI: G35
- SCK: G36
- CS: G37

The current driver initializes the ST7789 over ESP-IDF SPI, controls the
backlight, writes pixel buffers and provides basic drawing helpers for filled
rectangles, outlined rectangles, lines and compact 5x7 text. Offsets, rotation,
color order and partial update strategy still need hardware validation before
the display path should be treated as production-ready.
