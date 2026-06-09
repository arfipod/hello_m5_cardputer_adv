# Smoke Tests

The diagnostics firmware runs a boot smoke sequence and then polls keyboard
events.

## Expected Boot Checks

- System info
- I2C scan
- Keyboard detection
- Display backlight and placeholder draw calls
- ES8311 detection candidates
- I2S placeholder tone request
- microSD placeholder check
- BMI270 detection
- Battery ADC read
- IR GPIO init
- Optional LVGL hook status

## Manual COM5 Flow

```sh
pio run -e cardputer_adv -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```

Record observed I2C addresses before enabling full ES8311, BMI270 or TCA8418
register sequences.
