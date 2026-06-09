# Pinout

`src/hardware/BoardPins.hpp` is the code source of truth.

| Function | GPIO |
|---|---:|
| I2C SDA | G8 |
| I2C SCL | G9 |
| Keyboard INT | G11 |
| Battery ADC | G10 |
| SD CS | G12 |
| SD MOSI | G14 |
| SD CLK | G40 |
| SD MISO | G39 |
| LCD BL | G38 |
| LCD RST | G33 |
| LCD DC/RS | G34 |
| LCD MOSI/DAT | G35 |
| LCD SCK | G36 |
| LCD CS | G37 |
| I2S BCLK/SCLK | G41 |
| I2S DOUT to codec DSDIN | G42 |
| I2S LRCLK | G43 |
| I2S DIN from codec ASDOUT | G46 |
| IR TX | G44 |
| Grove | G1/G2 |

EXT bus pins are represented in `BoardPins.hpp`, including G3, G4, G5, G6,
G8, G9, G13, G14, G15, G39 and G40.
