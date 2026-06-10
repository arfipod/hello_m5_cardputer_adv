# Audio

- Codec: ES8311 over shared I2C G8/G9
- I2S BCLK/SCLK: G41
- I2S DOUT to codec DSDIN: G42
- I2S LRCLK: G43
- I2S DIN from codec ASDOUT: G46

Expected sequence:

1. Initialize shared I2C.
2. Probe ES8311 candidate address.
3. Configure ES8311 clocks, DAC/ADC path, volume and mute state.
4. Initialize I2S transport.
5. Play a low-volume generated tone.

Status: I2S transport is prepared. ES8311 register initialization is intentionally stubbed and returns `ESP_ERR_NOT_SUPPORTED`.

Smoke test:

```bash
pio run -e cardputer_adv_audio_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
