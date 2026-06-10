# Infrared

- TX pin: G44
- API: ESP-IDF RMT TX

Expected sequence:

1. Initialize RMT TX on G44.
2. Configure carrier, typically 38 kHz.
3. Transmit raw RMT symbols or a protocol encoder.

Status: raw symbol transmit is prepared. Protocol helpers are intentionally not included yet.

Smoke test:

```bash
pio run -e cardputer_adv_ir_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
