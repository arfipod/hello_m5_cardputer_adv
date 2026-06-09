# Flashing

Main upload command:

```sh
pio run -e cardputer_adv -t upload --upload-port COM5
```

Serial monitor:

```sh
pio device monitor -p COM5 -b 115200
```

If flashing fails with repeated `Connecting...` messages:

1. Disconnect/reconnect USB-C.
2. Put the device into download mode with the board's G0/boot procedure.
3. Retry the upload command.
4. If needed, lower `upload_speed` in `platformio.ini`.

CI must not flash hardware. Flashing belongs to local validation or a future
self-hosted runner with an attached device.
