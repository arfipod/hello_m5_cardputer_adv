# microSD

- Bus: SPI
- CS: G12
- MOSI: G14
- CLK: G40
- MISO: G39

Expected sequence:

1. Initialize SDSPI bus.
2. Mount FATFS at `/sdcard`.
3. List the root directory.
4. Optionally write/read a small file.

Status: mount/list path is prepared. Write/read is opt-in with `APP_SMOKE_SD_WRITE=1`.

Smoke test:

```bash
pio run -e cardputer_adv_sd_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
