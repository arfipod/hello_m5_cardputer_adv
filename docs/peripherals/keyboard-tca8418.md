# Keyboard: TCA8418

The keyboard is controlled by a TCA8418 over the shared I2C bus with interrupt
on G11.

The template separates:

- raw TCA8418 row/column events
- portable logical key mapping
- app-level command handling

The current mapper is intentionally conservative and host-tested. Verify the
complete Cardputer-Adv physical layout against the official demo and real key
events before treating it as final.
