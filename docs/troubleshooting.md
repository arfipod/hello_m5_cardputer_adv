# Troubleshooting

## `pio` Not Found

Install PlatformIO Core or use:

```sh
python -m platformio --version
```

## Native Tests Fail On Windows

`platform = native` requires a host C/C++ compiler such as GCC or MSVC. GitHub
Actions on Ubuntu provides this by default.

## Upload Fails

- Confirm the USB cable supports data.
- Confirm the correct COM port.
- Try the manual boot/download sequence.
- Lower `upload_speed` if the connection is unstable.

## No Serial Logs

This template uses USB Serial/JTAG console settings in `sdkconfig.defaults`.
Use 115200 baud and the port exposed by the ESP32-S3 USB connection.
