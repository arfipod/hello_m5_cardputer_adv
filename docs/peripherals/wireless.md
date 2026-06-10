# Wireless

WiFi and BLE services are intentionally optional and disabled by default.

Status: service shells only. No credentials, provisioning, scanning, GATT services or network tasks are implemented.

Expected future sequence:

1. Enable `APP_ENABLE_WIFI=1` or `APP_ENABLE_BLE=1`.
2. Add explicit service ownership in the app.
3. Store credentials through an intentional project-specific path, never in the template.
4. Add smoke tests for scan-only WiFi and BLE advertising/scanning.

Known risks:

- WiFi can contend with ADC reads.
- BLE stack selection affects RAM use.
- Network features can obscure peripheral bring-up, so validate hardware smoke tests first.
