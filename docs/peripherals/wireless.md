# Wireless

WiFi and BLE services are intentionally optional and disabled by default.

## WiFi

Status: basic ESP-IDF station service. `WifiService` can initialize NVS/netif/event loop/WiFi, start station mode, scan nearby access points, connect with caller-provided credentials and wait for an IPv4 address.

Credentials are never stored in the template. Pass them from an app-specific configuration layer, NVS provisioning, secure storage or temporary smoke-test build flags.

Smoke test:

```bash
pio run -e cardputer_adv_wifi_test
```

To test a real connection locally, pass credentials as temporary build flags instead of editing tracked files.

## BLE

Status: service shell only. BLE init and advertising hooks return `ESP_ERR_NOT_SUPPORTED` until the project chooses and wires a concrete host stack, GAP advertising payload and GATT services.

Expected future sequence:

1. Enable `APP_ENABLE_BLE=1`.
2. Choose NimBLE or Bluedroid for the target memory/features.
3. Add explicit service ownership in the app.
4. Add smoke tests for BLE advertising/scanning.

Known risks:

- WiFi can contend with ADC reads.
- BLE stack selection affects RAM use.
- Network features can obscure peripheral bring-up, so validate hardware smoke tests first.
