# Battery

- ADC pin: G10
- API: ESP-IDF ADC oneshot

Expected sequence:

1. Resolve ADC unit/channel from G10.
2. Configure ADC oneshot.
3. Read raw values.
4. Apply calibrated conversion once divider values are confirmed.

Status: raw ADC read is prepared. Voltage estimation returns `ESP_ERR_NOT_SUPPORTED` until calibration is added.

Smoke test:

```bash
pio run -e cardputer_adv_battery_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
