# USB

USB hooks are prepared for future USB CDC, MIDI and HID experiments.

Status: service shell only. No TinyUSB dependency or descriptors are added yet.

Expected future sequence:

1. Enable `APP_ENABLE_USB=1`.
2. Add ESP-IDF TinyUSB support or `esp_tinyusb` if suitable for the selected ESP-IDF version.
3. Add descriptors for the target USB class.
4. Validate flashing and serial-monitor behavior, because USB changes can affect developer workflow.

Smoke test status: pending. No USB device behavior is implemented.
