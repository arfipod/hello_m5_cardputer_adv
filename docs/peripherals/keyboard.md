# Keyboard

- Device: TCA8418
- Bus: shared I2C on G8/G9
- Interrupt: G11
- Address: `0x34`
- Matrix: prepared for 7 x 8

Expected sequence:

1. Initialize `I2CBus`.
2. Configure G11 as interrupt input.
3. Configure TCA8418 keypad GPIO registers.
4. Drain FIFO events.
5. Map raw row/column events to logical keys.

Status: partial. Raw event reading and placeholder mapping exist, but the exact Cardputer-Adv layout must be validated against hardware.

Smoke test:

```bash
pio run -e cardputer_adv_keyboard_test -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```
