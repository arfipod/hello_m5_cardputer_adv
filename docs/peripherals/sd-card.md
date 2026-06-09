# microSD

Pins:

- CS: G12
- MOSI: G14
- SCLK: G40
- MISO: G39

Start with FAT32 cards. The current abstraction is ready for SDSPI/FAT mount
support, but actual filesystem operations should be enabled after basic SPI
bring-up is confirmed.
