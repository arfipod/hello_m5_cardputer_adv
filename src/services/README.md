# Services

App-facing services that coordinate optional subsystems.

- `WifiService`: optional ESP-IDF station service for init, scan, connect and connection wait.
- `BleService`: explicit BLE contract; returns `ESP_ERR_NOT_SUPPORTED` until a host stack and GATT/GAP setup are added.
- `UsbService`: explicit USB device contract; returns `ESP_ERR_NOT_SUPPORTED` until TinyUSB descriptors and class callbacks are added.

Keep low-level board and peripheral details in `src/hardware`.
