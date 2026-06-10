# Battery

- ADC pin: G10
- API: ESP-IDF ADC oneshot

Expected sequence:

1. Resolve ADC unit/channel from G10.
2. Configure ADC oneshot.
3. Read raw values.
4. Apply ESP-IDF ADC calibration and the Cardputer/Cardputer-Adv 2.0 voltage ratio used by M5Unified.

Status: raw ADC read is prepared. Approximate battery voltage returns calibrated millivolts when the target supports ADC calibration. Charging status and current are not available on Cardputer/Cardputer-Adv hardware.

Smoke test:

```bash
pio run -e cardputer_adv_battery_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
