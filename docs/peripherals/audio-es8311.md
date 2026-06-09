# Audio: ES8311 And I2S

Control path:

- ES8311 over shared I2C G8/G9

I2S lines:

- BCLK: G41
- DOUT: G42
- LRCLK: G43
- DIN: G46

The template provides `Es8311Codec`, `I2SAudioDevice` and `AudioService`.
Register setup and DMA waveform output are intentionally marked as pending
until verified on real hardware.
