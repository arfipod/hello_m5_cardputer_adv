# Getting Started

## Requirements

- VS Code
- PlatformIO extension or PlatformIO Core
- USB-C data cable
- M5 Cardputer-Adv

## Build

```sh
pio run -e cardputer_adv
```

Fallback build:

```sh
pio run -e cardputer_adv_devkit_fallback
```

## Test

```sh
pio test -e native
```

## Upload

```sh
pio run -e cardputer_adv -t upload --upload-port COM5
```

This skeleton prints chip and flash information, then starts a generic app
heartbeat. Peripheral drivers are planned, not implemented yet.
