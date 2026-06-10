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

Optional WiFi smoke build:

```sh
pio run -e cardputer_adv_wifi_test
```

Full board smoke build:

```sh
pio run -e cardputer_adv_full_smoke_test
```

GitHub Actions runs all three commands. Hardware flashing is intentionally not
part of CI.
