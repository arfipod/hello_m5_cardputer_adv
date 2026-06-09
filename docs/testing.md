# Testing

Native tests validate portable code in `lib/common`.

```sh
pio test -e native
```

Firmware builds:

```sh
pio run -e cardputer_adv
pio run -e cardputer_adv_devkit_fallback
```

GitHub Actions runs all three commands. Hardware flashing is intentionally not
part of CI.
