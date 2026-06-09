# M5Stack Cardputer-Adv Development Guide

**Target stack:** VS Code + PlatformIO + ESP-IDF  
**Target hardware:** M5Stack Cardputer-Adv / Stamp-S3A / ESP32-S3FN8  
**Purpose:** hardware and firmware reference for building robust embedded applications on the Cardputer-Adv.  
**Scope:** general development guide. This guide intentionally avoids focusing on any single application idea.

---

## 1. What the Cardputer-Adv is

The M5Stack Cardputer-Adv is a programmable, card-sized ESP32-S3 device built around the **Stamp-S3A** core module. It combines a keyboard, display, audio subsystem, storage, battery, IMU, infrared emitter, Grove connector and expansion bus into a compact embedded development platform.

Think of it as:

```text
ESP32-S3 embedded board
+ keyboard
+ small LCD
+ audio codec / speaker / jack
+ microphone
+ IMU
+ microSD
+ battery
+ Grove / expansion bus
```

It is not a Linux computer. There is no operating system with processes, filesystems and drivers in the desktop sense. You normally write a firmware application using **ESP-IDF**, optionally orchestrated through **PlatformIO**, and run it directly on the ESP32-S3 under FreeRTOS.

---

## 2. Main hardware capabilities

| Area | Hardware | What it enables |
|---|---|---|
| MCU | ESP32-S3FN8, dual-core Xtensa LX7, up to 240 MHz | Embedded apps, Wi-Fi/BLE, USB, FreeRTOS tasks, real-time firmware |
| Flash | 8 MB | Firmware, assets, configuration partitions |
| Display | ST7789V2, 1.14", 240 x 135 px | UI, dashboards, status screens, menus, graphics |
| Keyboard | 56 keys, 4 x 14 logical layout, TCA8418 IO expander | Text input, shortcuts, command palettes, HID-like input, app control |
| Audio codec | ES8311 | Digital audio input/output via I2S + I2C control |
| Amplifier | NS4150B + 8 ohm / 1 W speaker | Built-in sound output |
| Jack | 3.5 mm audio output | External headphones/speakers; speaker amp is disabled when jack is inserted |
| Microphone | MEMS microphone | Audio capture, voice, metering, signal analysis |
| IMU | BMI270 6-axis motion sensor | Motion input, gestures, orientation, activity detection |
| Storage | microSD over SPI | logs, assets, files, configuration, recorded data |
| IR | IR emitter on G44 | IR remote control / transmission |
| Battery | 1750 mAh Li-ion, ADC on G10 | Portable apps, battery monitoring |
| Grove | HY2.0-4P custom port | External I2C/GPIO peripherals |
| EXT bus | 2.54 mm 14-pin expansion bus | SPI, I2C, UART, 5V/5VOUT/GND, external modules |

---

## 3. Development stack

Recommended stack for serious firmware work:

```text
VS Code
  ↓
PlatformIO extension / PlatformIO Core
  ↓
ESP-IDF framework
  ↓
CMake / Ninja / esptool
  ↓
ESP32-S3 firmware
```

### Why this stack

- **VS Code** gives a convenient IDE, terminal, Git integration and PlatformIO integration.
- **PlatformIO** gives reproducible environments, board profiles, upload/monitor commands, tests and CI-friendly workflows.
- **ESP-IDF** gives direct access to the real ESP32-S3 capabilities: FreeRTOS, I2C, I2S, SPI, Wi-Fi, BLE, NVS, USB, power management and low-level APIs.

### What not to hide from yourself

PlatformIO is convenient, but ESP-IDF is still underneath. You should understand:

```text
platformio.ini
CMakeLists.txt
src/CMakeLists.txt
sdkconfig
sdkconfig.defaults
app_main()
esptool flashing
bootloader.bin
partitions.bin
firmware.bin
firmware.elf
```

---

## 4. PlatformIO board profile recommendation

The Cardputer-Adv documentation provides a PlatformIO snippet for Arduino using:

```ini
board = esp32-s3-devkitc-1
framework = arduino
```

For an **ESP-IDF-oriented project**, a practical setup is to use:

```ini
board = m5stack-stamps3
framework = espidf
```

because the Cardputer-Adv is Stamp-S3A/ESP32-S3 based. Keep a fallback environment using `esp32-s3-devkitc-1` because M5Stack’s official Arduino example uses that generic ESP32-S3 board profile.

Recommended `platformio.ini` baseline:

```ini
[platformio]
default_envs = cardputer_adv

[env:cardputer_adv]
platform = espressif32
board = m5stack-stamps3
framework = espidf
upload_protocol = esptool
upload_speed = 921600
monitor_speed = 115200
board_build.mcu = esp32s3
board_build.f_cpu = 240000000L
board_build.flash_size = 8MB
build_flags =
    -DAPP_TARGET_CARDPUTER_ADV=1

[env:cardputer_adv_devkit_fallback]
platform = espressif32
board = esp32-s3-devkitc-1
framework = espidf
upload_protocol = esptool
upload_speed = 921600
monitor_speed = 115200
board_build.mcu = esp32s3
board_build.f_cpu = 240000000L
board_build.flash_size = 8MB
build_flags =
    -DAPP_TARGET_CARDPUTER_ADV=1
    -DAPP_BOARD_FALLBACK=1
```

This is not the only valid setup, but it is a good learning-oriented baseline.

---

## 5. Minimal ESP-IDF project structure under PlatformIO

PlatformIO ESP-IDF projects usually need both PlatformIO metadata and ESP-IDF CMake files:

```text
project-root/
├── platformio.ini
├── CMakeLists.txt
├── sdkconfig.defaults
├── include/
├── src/
│   ├── CMakeLists.txt
│   └── main.cpp
├── lib/
├── test/
├── docs/
└── .github/
```

Root `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(cardputer_adv_lab)
```

`src/CMakeLists.txt`:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
)
```

Minimal `main.cpp`:

```cpp
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "cardputer";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Cardputer-Adv firmware starting...");

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    uint32_t flash_size = 0;
    esp_flash_get_size(nullptr, &flash_size);

    ESP_LOGI(TAG, "CPU cores: %d", chip_info.cores);
    ESP_LOGI(TAG, "Flash size: %lu MB", flash_size / (1024 * 1024));

    uint32_t counter = 0;
    while (true) {
        ESP_LOGI(TAG, "heartbeat=%lu", counter++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

---

## 6. Build, upload and monitor

Build:

```bash
pio run -e cardputer_adv
```

Upload:

```bash
pio run -e cardputer_adv -t upload
```

Serial monitor:

```bash
pio device monitor -b 115200
```

ESP-IDF menuconfig through PlatformIO:

```bash
pio run -e cardputer_adv -t menuconfig
```

Fallback build:

```bash
pio run -e cardputer_adv_devkit_fallback
```

Fallback upload:

```bash
pio run -e cardputer_adv_devkit_fallback -t upload
```

---

## 7. Download / bootloader mode

If flashing fails with repeated `Connecting...` messages, force download mode manually:

```text
1. Connect USB-C.
2. Put the side power switch in OFF.
3. Hold G0.
4. Power the device ON / apply power.
5. Release G0.
6. Run the upload command again.
```

`G0` is the boot/download-related pin exposed by the Stamp-S3 module. The Stamp-S3 schematic also exposes `EN`, which is the reset/enable line. In practice, automatic reset/upload may work, but knowing the manual G0 sequence is essential.

---

## 8. Schematic-derived pinout reference

Create a single source of truth in code, for example:

```text
src/hardware/BoardPins.hpp
```

Recommended contents:

```cpp
#pragma once

#include "driver/gpio.h"

namespace board {

// Shared I2C bus
static constexpr gpio_num_t I2C_SDA = GPIO_NUM_8;
static constexpr gpio_num_t I2C_SCL = GPIO_NUM_9;

// Keyboard: TCA8418
static constexpr gpio_num_t KEYBOARD_INT = GPIO_NUM_11;

// Battery
static constexpr gpio_num_t BATTERY_ADC = GPIO_NUM_10;

// microSD SPI
static constexpr gpio_num_t SD_CS   = GPIO_NUM_12;
static constexpr gpio_num_t SD_MOSI = GPIO_NUM_14;
static constexpr gpio_num_t SD_SCLK = GPIO_NUM_40;
static constexpr gpio_num_t SD_MISO = GPIO_NUM_39;

// Display SPI
static constexpr gpio_num_t LCD_RST  = GPIO_NUM_33;
static constexpr gpio_num_t LCD_DC   = GPIO_NUM_34;
static constexpr gpio_num_t LCD_MOSI = GPIO_NUM_35;
static constexpr gpio_num_t LCD_SCLK = GPIO_NUM_36;
static constexpr gpio_num_t LCD_CS   = GPIO_NUM_37;
static constexpr gpio_num_t LCD_BL   = GPIO_NUM_38;

// Audio ES8311 / I2S
static constexpr gpio_num_t I2S_BCLK  = GPIO_NUM_41;
static constexpr gpio_num_t I2S_DOUT  = GPIO_NUM_42; // ESP32-S3 -> ES8311 DSDIN
static constexpr gpio_num_t I2S_LRCLK = GPIO_NUM_43;
static constexpr gpio_num_t I2S_DIN   = GPIO_NUM_46; // ES8311 ASDOUT -> ESP32-S3

// IR
static constexpr gpio_num_t IR_TX = GPIO_NUM_44;

// Boot
static constexpr gpio_num_t BOOT = GPIO_NUM_0;

}
```

Do not scatter pin numbers throughout drivers. Keep this header as the board abstraction boundary.

---

## 9. Shared I2C bus

The Cardputer-Adv uses a shared I2C bus:

```text
G8  → SDA
G9  → SCL
```

Devices on this bus include:

```text
TCA8418 keyboard controller
ES8311 audio codec control interface
BMI270 IMU
Grove I2C devices if connected
```

Recommended design:

```text
I2CBus
├── init once
├── scan devices
├── read register
├── write register
├── read bytes
├── write bytes
└── mutex-protected transactions if accessed from multiple tasks
```

Start with an **I2C scanner**. It is one of the best early bring-up tools because it confirms real hardware presence before writing full drivers.

Expected known devices:

| Device | Bus | Notes |
|---|---|---|
| TCA8418 | G8/G9 | Keyboard controller, 7-bit address 0x34 according to schematic/demo context |
| BMI270 | G8/G9 | IMU, 7-bit address 0x69 according to schematic context |
| ES8311 | G8/G9 | Audio codec control interface; confirm address by scan/datasheet/demo |

---

## 10. Keyboard subsystem: TCA8418

The keyboard is not a direct GPIO matrix from the ESP32-S3. It is managed by a **TCA8418** keyboard controller / IO expander over I2C, with interrupt on G11.

Hardware path:

```text
56 physical keys
    ↓
TCA8418 matrix scanner
    ↓ I2C G8/G9 + INT G11
ESP32-S3 firmware
```

Recommended firmware layers:

```text
Tca8418Keyboard
├── initialize TCA8418
├── configure matrix
├── configure interrupt
├── read event FIFO
├── produce raw row/col events

KeyboardLayout
├── remap physical row/col to logical layout
├── apply Fn/Shift/Ctrl/Alt/Meta behavior
├── generate high-level key events

ApplicationInput
├── route key events to UI, commands, text input or other app logic
```

The official Cardputer-Adv demo branch uses a `Keyboard` class around `Adafruit_TCA8418`, configures a 7x8 matrix, attaches a GPIO interrupt, reads TCA8418 events, remaps the row/column coordinates and converts them to HID-like keycodes. Use it as a reference, but do not blindly copy its architecture if you want a smaller and cleaner learning repo.

Important design rule:

```text
Do not directly couple physical key row/col to application behavior.
```

Always separate:

```text
raw hardware event → logical key event → application command
```

---

## 11. Display subsystem: ST7789V2

The display is a small SPI LCD:

```text
ST7789V2
240 x 135 px
1.14 inch
```

Pins:

```text
BL   → G38
RST  → G33
DC/RS→ G34
MOSI → G35
SCLK → G36
CS   → G37
```

Possible development approaches:

### Direct display driver

Use ESP-IDF display/SPI APIs or a small ST7789 component. This gives best learning value because you understand:

```text
SPI bus setup
LCD reset sequence
backlight control
pixel formats
frame buffers / partial updates
render loop timing
```

### M5 / higher-level graphics library

Useful for faster UI work, but can hide initialization details.

### LVGL

Good for mature UI: screens, buttons, lists, sliders, styles, focus/navigation. Best introduced after you understand basic display bring-up.

Recommended progression:

```text
1. Turn backlight on/off.
2. Clear screen with one color.
3. Draw text.
4. Draw lines/rectangles.
5. Draw a status page.
6. Add partial refresh strategy.
7. Add LVGL only when the low-level path is understood.
```

---

## 12. Audio subsystem: ES8311 + I2S + NS4150B + jack

Cardputer-Adv has a more capable audio path than the earlier Cardputer variants.

Hardware path:

```text
ESP32-S3 I2S
    ↓
ES8311 codec
    ↓
NS4150B amplifier → internal speaker
    ↓
3.5 mm jack path for external headphones/speakers
```

Control path:

```text
ESP32-S3 I2C G8/G9 → ES8311 registers
```

I2S pins:

```text
G41 → SCLK / BCLK
G43 → LRCK / WS
G42 → DSDIN  : ESP32-S3 audio output to codec
G46 → ASDOUT : codec audio input to ESP32-S3
```

Important naming convention:

```text
From ESP32-S3 perspective:
I2S_DOUT = G42  // data going out from ESP into codec
I2S_DIN  = G46  // data coming in from codec into ESP
```

Recommended layers:

```text
Es8311Codec
├── reset/configure codec registers
├── set volume/gain
├── configure input/output paths

I2SAudioDevice
├── configure ESP-IDF I2S channel
├── transmit audio samples
├── receive microphone samples if needed

AudioService
├── owns buffers
├── handles sample rate / format
├── avoids blocking the rest of the system
```

Bring-up order:

```text
1. I2C scan: detect codec.
2. Codec init: write/read known registers if possible.
3. I2S init with conservative sample rate.
4. Generate simple known waveform / tone / sample pattern.
5. Confirm output through speaker.
6. Confirm output through 3.5 mm jack.
7. Confirm speaker amp behavior when jack is inserted.
8. Add microphone capture only after output is stable.
```

Good initial audio parameters:

```text
Sample rate: 16000 Hz, 22050 Hz or 44100 Hz
Format: signed 16-bit
Channels: mono duplicated to stereo if required, or stereo if codec path expects it
Buffer size: start conservative, then tune latency
```

Real-time rules:

```text
No logging inside tight audio loops.
No malloc/free inside the audio callback/task loop.
No display drawing inside audio generation.
Use DMA/I2S buffers correctly.
Keep UI and storage operations outside the audio timing path.
```

---

## 13. Microphone

The MEMS microphone connects through the ES8311 codec path. Treat microphone support as part of the audio subsystem, not as a separate ADC problem.

Possible uses:

```text
audio recording
level meter
voice detection
signal visualization
frequency analysis
wake-word experiments, if resources allow
```

Recommended bring-up:

```text
1. Confirm ES8311 output path first.
2. Configure ES8311 input path.
3. Capture I2S samples from G46.
4. Compute RMS level.
5. Display a simple level meter.
6. Store short captures to microSD if needed.
```

---

## 14. IMU subsystem: BMI270

The BMI270 is connected over the shared I2C bus:

```text
SDA → G8
SCL → G9
```

Possible uses:

```text
orientation
motion gestures
tap/double-tap detection
activity sensing
screen rotation logic
motion-controlled UI interactions
portable device behavior
```

Recommended driver layers:

```text
Bmi270Imu
├── init
├── read accelerometer
├── read gyroscope
├── configure data rate / range
├── expose calibrated values

MotionService
├── filtering
├── gesture detection
├── orientation state
```

Be careful not to poll the IMU too aggressively if it shares the I2C bus with keyboard and codec control.

---

## 15. microSD subsystem

microSD is connected over SPI:

```text
CS   → G12
MOSI → G14
CLK  → G40
MISO → G39
```

Possible uses:

```text
logs
settings
assets
small databases
firmware resources
recorded data
configuration profiles
UI images/fonts
```

Recommended approach:

```text
SdCardStorage
├── mount/unmount
├── check card presence where possible
├── read/write files
├── list directories
├── expose errors clearly
```

Filesystem recommendation:

```text
Start with FAT32-formatted microSD cards.
Use 16 GB or 32 GB cards for maximum compatibility during bring-up.
Avoid depending on exFAT until the stack is explicitly configured and tested.
```

Avoid doing SD file I/O from timing-sensitive tasks. Storage can block.

---

## 16. Battery and power

The battery sense path is exposed on:

```text
Battery ADC → G10
```

Possible uses:

```text
battery voltage display
low-battery warning
adaptive brightness
low-power mode
runtime estimation
charging/status UI
```

Recommended layers:

```text
BatteryMonitor
├── configure ADC
├── read raw value
├── convert to voltage using board-specific scaling
├── filter readings
├── estimate percentage conservatively
```

Do not assume a linear voltage-to-percentage relationship is accurate. Li-ion discharge curves are not linear. Start by showing voltage; add percentage later if needed.

Charging note: official documentation says to switch power to `ON` when charging.

---

## 17. Infrared emitter

IR TX uses:

```text
G44 → IR transmitter
```

Possible uses:

```text
IR remote control
learning fixed IR protocols
sending NEC/Sony/RC5-like signals
appliance control experiments
```

In ESP-IDF, the RMT peripheral is usually a good fit for precise pulse generation. Keep timing-critical IR generation separated from UI code.

---

## 18. Grove and EXT expansion

### Grove HY2.0-4P

Official pinout:

```text
Black  → GND
Red    → 5V
Yellow → G2
White  → G1
```

This can be used for custom Grove peripherals, but always confirm voltage expectations.

### EXT 2.54-14P expansion bus

Official functions:

```text
1  RESET / G3       2  5VIN
3  INT   / G4       4  GND
5  BUSY  / G6       6  5VOUT
7  SCK   / G40      8  G8  / I2C_SDA
9  MOSI  / G14      10 G9  / I2C_SCL
11 MISO  / G39      12 G13 / UART_RX
13 CS    / G5       14 G15 / UART_TX
```

Important caveat: some expansion-bus pins are shared with onboard peripherals.

Examples:

```text
G8/G9 are shared with onboard I2C devices.
G40/G14/G39 are used by microSD SPI.
```

Plan pin ownership carefully.

---

## 19. Wireless capabilities

Because the board is ESP32-S3 based, it can support typical ESP-IDF wireless features:

```text
Wi-Fi station/AP
BLE
ESP-NOW
HTTP server/client
MQTT
WebSocket
NTP time sync
OTA updates
```

Possible application classes:

```text
portable Wi-Fi tools
IoT dashboards
sensor gateways
BLE controllers
local web configuration portals
remote control devices
telemetry loggers
```

Design note: Wi-Fi/BLE can increase power draw and can affect timing. Keep time-sensitive work isolated in dedicated tasks and measure behavior under radio load.

---

## 20. USB capabilities

The ESP32-S3 supports USB-related workflows. On this board, USB is essential for:

```text
flashing firmware
serial monitor
logs
possibly USB device features depending on firmware stack
```

Potential advanced uses:

```text
USB CDC serial
USB HID keyboard/mouse-like devices
USB MIDI-like experiments
Mass-storage-like workflows, if implemented carefully
```

USB features need deliberate ESP-IDF/TinyUSB configuration and should be introduced after the base firmware and normal flashing workflow are stable.

---

## 21. FreeRTOS architecture patterns

ESP-IDF applications run on FreeRTOS. For non-trivial Cardputer-Adv apps, avoid a single giant loop.

A good general architecture:

```text
main/app task
├── initializes hardware
├── starts services
├── owns high-level application state

input task
├── reads keyboard events
├── reads buttons / gestures
├── publishes events

ui task
├── draws display
├── handles menus/screens
├── runs LVGL if used

audio task, if needed
├── handles I2S buffers
├── avoids blocking

storage task
├── microSD file I/O
├── configuration save/load

wireless task/service
├── Wi-Fi/BLE/MQTT/HTTP
```

Communication tools:

```text
FreeRTOS queues
Event groups
Mutexes
Semaphores
Ring buffers
Task notifications
```

Design rules:

```text
Keep interrupt handlers short.
Do not block inside ISRs.
Use queues/task notifications to defer work.
Protect shared I2C bus access.
Keep storage and display work out of time-critical paths.
```

---

## 22. LVGL: when and how to use it

LVGL is a good option for rich UI:

```text
screens
buttons
lists
sliders
labels
text areas
menus
styles
animations
focus navigation
keyboard-driven UI
```

Recommended approach:

```text
1. Bring up display without LVGL first.
2. Bring up keyboard without LVGL first.
3. Add LVGL as a UI layer.
4. Map keyboard events into LVGL input devices / groups.
5. Keep LVGL out of real-time paths such as audio or strict timing protocols.
```

A clean LVGL integration should have:

```text
LvglPort
├── init display buffer
├── flush callback
├── tick handling
├── input device registration
├── periodic lv_timer_handler() call

UiScreens
├── home screen
├── settings screen
├── status screen
├── diagnostics screen
```

---

## 23. Testing strategy

Use several levels of tests.

### 23.1 Native host tests

Run on your PC / GitHub Actions. Best for portable logic:

```text
parsers
state machines
mappers
math utilities
configuration serialization
layout mapping
protocol framing
```

Example PlatformIO native environment:

```ini
[env:native]
platform = native
test_framework = unity
build_flags =
    -std=c++17
    -DAPP_HOST_TEST=1
```

### 23.2 Firmware build tests

Run in CI:

```bash
pio run -e cardputer_adv
pio run -e cardputer_adv_devkit_fallback
```

### 23.3 Hardware smoke tests

Run locally with the Cardputer connected:

```text
serial log test
I2C scan test
keyboard event test
display fill/text test
audio output test
microSD mount test
IMU read test
battery ADC test
IR transmit test
```

### 23.4 Embedded unit tests

ESP-IDF supports Unity-based tests on target. Use them when hardware behavior needs to be tested on-device.

---

## 24. GitHub Actions CI

CI should not try to flash the board unless you use a self-hosted runner with hardware attached.

Recommended CI:

```text
pull_request / push
├── install PlatformIO
├── run native tests
├── build firmware
├── build fallback firmware
└── upload artifacts
```

Example workflow:

```yaml
name: CI

on:
  push:
    branches: [main, develop]
  pull_request:
  workflow_dispatch:

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v6

      - uses: actions/cache@v4
        with:
          path: |
            ~/.cache/pip
            ~/.platformio/.cache
          key: ${{ runner.os }}-pio-${{ hashFiles('platformio.ini') }}
          restore-keys: |
            ${{ runner.os }}-pio-

      - uses: actions/setup-python@v6
        with:
          python-version: '3.11'

      - name: Install PlatformIO
        run: pip install --upgrade platformio

      - name: Run native tests
        run: pio test -e native

      - name: Build main firmware
        run: pio run -e cardputer_adv

      - name: Build fallback firmware
        run: pio run -e cardputer_adv_devkit_fallback

      - name: Upload firmware artifacts
        uses: actions/upload-artifact@v4
        with:
          name: cardputer-adv-firmware
          path: |
            .pio/build/cardputer_adv/firmware.bin
            .pio/build/cardputer_adv/bootloader.bin
            .pio/build/cardputer_adv/partitions.bin
            .pio/build/cardputer_adv/firmware.elf
```

---

## 25. Recommended repository structure

```text
repo/
├── .github/
│   └── workflows/
│       └── ci.yml
├── .vscode/
│   └── extensions.json
├── docs/
│   ├── cardputer-adv-hardware.md
│   ├── flashing.md
│   ├── architecture.md
│   ├── testing.md
│   ├── peripherals/
│   │   ├── i2c.md
│   │   ├── keyboard-tca8418.md
│   │   ├── display-st7789.md
│   │   ├── audio-es8311.md
│   │   ├── sd-card.md
│   │   ├── imu-bmi270.md
│   │   ├── battery.md
│   │   └── ir.md
├── include/
├── lib/
│   └── app_core/
│       ├── include/
│       └── src/
├── src/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── app/
│   ├── hardware/
│   │   ├── BoardPins.hpp
│   │   ├── I2CBus.hpp
│   │   ├── I2CBus.cpp
│   │   ├── Tca8418Keyboard.hpp
│   │   ├── Tca8418Keyboard.cpp
│   │   ├── Es8311Codec.hpp
│   │   ├── Es8311Codec.cpp
│   │   ├── I2SAudioDevice.hpp
│   │   ├── I2SAudioDevice.cpp
│   │   ├── St7789Display.hpp
│   │   ├── St7789Display.cpp
│   │   ├── SdCardStorage.hpp
│   │   ├── SdCardStorage.cpp
│   │   ├── Bmi270Imu.hpp
│   │   ├── Bmi270Imu.cpp
│   │   ├── BatteryMonitor.hpp
│   │   ├── BatteryMonitor.cpp
│   │   ├── IrTransmitter.hpp
│   │   └── IrTransmitter.cpp
│   └── ui/
├── test/
├── CMakeLists.txt
├── platformio.ini
├── sdkconfig.defaults
└── README.md
```

This structure is intentionally explicit. For learning, explicit files are often better than large glob-based builds.

---

## 26. Suggested learning order

A good order for mastering the board:

```text
1. PlatformIO + ESP-IDF build and flashing
2. Serial logs and app_main
3. sdkconfig/menuconfig
4. BoardPins.hpp from schematics
5. Shared I2C bus scanner
6. TCA8418 keyboard events
7. Display backlight and ST7789 clear screen
8. Display text and simple graphics
9. ES8311 detection over I2C
10. I2S output through codec
11. Microphone capture
12. microSD mount/read/write
13. BMI270 IMU readings
14. Battery ADC
15. IR transmitter
16. Grove/EXT external peripherals
17. Wi-Fi/BLE services
18. LVGL UI integration
19. On-device tests and diagnostics menu
20. Power management and robustness
```

The key is to close each step with:

```text
code
validation command
manual test
short documentation note
small commit
```

---

## 27. Diagnostics firmware idea

Before building larger applications, create a diagnostics firmware with a simple menu:

```text
Diagnostics
├── System info
├── I2C scan
├── Keyboard test
├── Display test
├── Audio output test
├── Microphone level
├── SD card test
├── IMU test
├── Battery voltage
├── IR test
└── Expansion bus notes
```

This becomes your hardware confidence tool. It also prevents guessing when later features fail.

---

## 28. Common pitfalls

### Upload fails

Try lower upload speed:

```ini
upload_speed = 460800
```

Then force download mode with G0.

### Display remains black

Check:

```text
backlight G38
SPI pins G35/G36/G37
reset G33
DC/RS G34
power switch
LCD init sequence
```

### Keyboard gives no events

Check:

```text
I2C bus G8/G9
TCA8418 address 0x34
INT pin G11
interrupt configuration
TCA8418 FIFO flush/read
row/col remap
```

### Audio is silent

Check:

```text
I2C codec configuration
ES8311 output path
I2S pins G41/G42/G43/G46
sample format
speaker vs jack behavior
volume/gain
amp enable path
```

### I2C bus becomes unstable

Check:

```text
mutex usage
clock speed
pull-ups
external Grove devices
long transactions
access from multiple tasks
```

### microSD fails

Check:

```text
FAT32 format
CS G12
MOSI G14
CLK G40
MISO G39
SPI bus conflicts
card quality
```

### CI passes but hardware fails

Expected. CI can build and test portable logic, but only real hardware validates pinout, timing, power and peripherals.

---

## 29. What the Cardputer-Adv is especially good for

General categories of applications:

```text
portable embedded UI devices
keyboard-driven tools
wireless configuration terminals
IoT dashboards
BLE/Wi-Fi utilities
IR controllers
sensor data loggers
audio input/output experiments
motion-controlled interfaces
microSD-based portable tools
USB HID-style devices
field diagnostics tools
small graphical applications
education / embedded learning labs
```

Its main strength is not raw compute. Its strength is that it combines many real embedded subsystems in a compact device:

```text
input + display + audio + storage + battery + sensors + wireless + expansion
```

That makes it excellent for learning complete embedded product architecture.

---

## 30. What it is not ideal for

Avoid expecting it to behave like:

```text
a Linux computer
a desktop audio workstation
a high-resolution graphics device
a high-memory edge AI box
a high-performance browser device
a general-purpose laptop replacement
```

You can do surprisingly capable things, but the design must respect microcontroller constraints:

```text
limited RAM
limited flash
timing-sensitive peripherals
shared buses
battery limits
small display
no desktop OS
```

---

## 31. Reference sources consulted

- M5Stack Cardputer-Adv official documentation.
- M5Stack Cardputer-Adv schematic PDF.
- M5Stack Stamp-S3 schematic PDF.
- M5Stack M5Cardputer-UserDemo branch `CardputerADV`, especially the keyboard HAL.
- PlatformIO ESP-IDF framework documentation.
- PlatformIO GitHub Actions integration documentation.
- PlatformIO M5Stack StampS3 board page.
- Espressif ESP-IDF I2C documentation.
- Espressif ESP-IDF I2S documentation.

---

## 32. Practical recommendation

For a serious Cardputer-Adv ESP-IDF repository, start with:

```text
1. platformio.ini with main + fallback envs
2. minimal app_main with logs
3. BoardPins.hpp from schematic
4. docs/hardware/cardputer-adv.md
5. I2C scanner
6. keyboard bring-up
7. display bring-up
8. audio codec/I2S bring-up
9. tests + GitHub Actions
```

Do not begin with a large UI framework, many features or copied factory demo code. Start small, verify each subsystem, document what is known, and keep the hardware abstraction clean.
