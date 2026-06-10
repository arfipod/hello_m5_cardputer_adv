#include "apps/smoke_tests/SmokeTests.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "dirent.h"
#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hardware/BatteryMonitor.hpp"
#include "hardware/Bmi270Imu.hpp"
#include "hardware/Es8311Codec.hpp"
#include "hardware/I2CBus.hpp"
#include "hardware/I2SAudio.hpp"
#include "hardware/IrTransmitter.hpp"
#include "hardware/SdCard.hpp"
#include "hardware/St7789Display.hpp"
#include "hardware/Tca8418Keyboard.hpp"
#include "services/BleService.hpp"
#include "services/UsbService.hpp"
#include "services/WifiService.hpp"
#include "ui/LvglPort.hpp"

#ifndef APP_MODE_BOOT_INFO
#define APP_MODE_BOOT_INFO 0
#endif
#ifndef APP_MODE_I2C_SCAN
#define APP_MODE_I2C_SCAN 0
#endif
#ifndef APP_MODE_KEYBOARD_TEST
#define APP_MODE_KEYBOARD_TEST 0
#endif
#ifndef APP_MODE_DISPLAY_TEST
#define APP_MODE_DISPLAY_TEST 0
#endif
#ifndef APP_MODE_AUDIO_TEST
#define APP_MODE_AUDIO_TEST 0
#endif
#ifndef APP_MODE_SD_TEST
#define APP_MODE_SD_TEST 0
#endif
#ifndef APP_MODE_IMU_TEST
#define APP_MODE_IMU_TEST 0
#endif
#ifndef APP_MODE_BATTERY_TEST
#define APP_MODE_BATTERY_TEST 0
#endif
#ifndef APP_MODE_IR_TEST
#define APP_MODE_IR_TEST 0
#endif
#ifndef APP_MODE_LVGL_DEMO
#define APP_MODE_LVGL_DEMO 0
#endif
#ifndef APP_MODE_WIFI_TEST
#define APP_MODE_WIFI_TEST 0
#endif
#ifndef APP_MODE_FULL_SMOKE_TEST
#define APP_MODE_FULL_SMOKE_TEST 0
#endif
#ifndef APP_SMOKE_SD_WRITE
#define APP_SMOKE_SD_WRITE 0
#endif
#ifndef APP_FULL_SMOKE_KEYBOARD_MS
#define APP_FULL_SMOKE_KEYBOARD_MS 10000
#endif
#ifndef APP_WIFI_SMOKE_SSID
#define APP_WIFI_SMOKE_SSID ""
#endif
#ifndef APP_WIFI_SMOKE_PASSWORD
#define APP_WIFI_SMOKE_PASSWORD ""
#endif

namespace cardputer::app::smoke_tests {
namespace {
constexpr const char* TAG = "smoke";

enum class CheckStatus : uint8_t {
    Pass,
    Warn,
    Fail,
};

struct SmokeSummary {
    uint16_t passed = 0;
    uint16_t warnings = 0;
    uint16_t failed = 0;
};

void recordCheck(SmokeSummary& summary, CheckStatus status, const char* name, const char* detail) {
    switch (status) {
        case CheckStatus::Pass:
            ++summary.passed;
            ESP_LOGI(TAG, "[PASS] %s: %s", name, detail);
            break;
        case CheckStatus::Warn:
            ++summary.warnings;
            ESP_LOGW(TAG, "[WARN] %s: %s", name, detail);
            break;
        case CheckStatus::Fail:
            ++summary.failed;
            ESP_LOGE(TAG, "[FAIL] %s: %s", name, detail);
            break;
    }
}

void recordErr(SmokeSummary& summary, CheckStatus ok_status, const char* name, esp_err_t err) {
    recordCheck(summary, err == ESP_OK ? ok_status : CheckStatus::Fail, name, esp_err_to_name(err));
}

void idleLoop(const char* mode) {
    uint32_t heartbeat = 0;
    while (true) {
        ESP_LOGI(TAG, "%s heartbeat=%lu", mode, static_cast<unsigned long>(heartbeat++));
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

const char* resetReasonName(esp_reset_reason_t reason) {
    switch (reason) {
        case ESP_RST_POWERON:
            return "poweron";
        case ESP_RST_EXT:
            return "external";
        case ESP_RST_SW:
            return "software";
        case ESP_RST_PANIC:
            return "panic";
        case ESP_RST_INT_WDT:
            return "interrupt_watchdog";
        case ESP_RST_TASK_WDT:
            return "task_watchdog";
        case ESP_RST_WDT:
            return "watchdog";
        case ESP_RST_DEEPSLEEP:
            return "deep_sleep";
        case ESP_RST_BROWNOUT:
            return "brownout";
        case ESP_RST_SDIO:
            return "sdio";
        default:
            return "unknown";
    }
}

[[maybe_unused]] void runBootInfo() {
    esp_chip_info_t chip_info{};
    esp_chip_info(&chip_info);
    uint32_t flash_size = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_flash_get_size(nullptr, &flash_size));

    ESP_LOGI(TAG, "mode=boot_info");
    ESP_LOGI(TAG, "cores=%d features=0x%lx revision=%d", chip_info.cores,
             static_cast<unsigned long>(chip_info.features), chip_info.revision);
    ESP_LOGI(TAG, "flash_size=%lu bytes", static_cast<unsigned long>(flash_size));
    ESP_LOGI(TAG, "free_heap=%lu internal_free=%lu",
             static_cast<unsigned long>(esp_get_free_heap_size()),
             static_cast<unsigned long>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL)));
    ESP_LOGI(TAG, "reset_reason=%s", resetReasonName(esp_reset_reason()));
    idleLoop("boot_info");
}

bool containsAddress(const std::array<uint8_t, 128>& addresses, size_t count, uint8_t needle) {
    for (size_t index = 0; index < count; ++index) {
        if (addresses[index] == needle) {
            return true;
        }
    }
    return false;
}

void drawDisplayPattern(hardware::St7789Display& display, SmokeSummary& summary) {
    constexpr uint16_t colors[] = {0xF800, 0x07E0, 0x001F, 0xFFE0, 0xFFFF, 0x0000};
    constexpr uint16_t bar_width = hardware::St7789Display::kWidth / (sizeof(colors) / sizeof(colors[0]));
    constexpr uint16_t bar_height = 36;
    esp_err_t err = ESP_OK;
    err = display.clear(0x0000);
    if (err != ESP_OK) {
        recordErr(summary, CheckStatus::Pass, "display clear", err);
        return;
    }
    for (size_t index = 0; index < sizeof(colors) / sizeof(colors[0]); ++index) {
        err = display.drawFilledRect(static_cast<uint16_t>(index * bar_width), 0, bar_width, bar_height, colors[index]);
        if (err != ESP_OK) {
            recordErr(summary, CheckStatus::Pass, "display color bars", err);
            return;
        }
    }

    (void)display.drawRect(0, 0, hardware::St7789Display::kWidth, hardware::St7789Display::kHeight, 0xFFFF);
    (void)display.drawFilledRect(0, 0, 8, 8, 0xF800);
    (void)display.drawFilledRect(hardware::St7789Display::kWidth - 8, 0, 8, 8, 0x07E0);
    (void)display.drawFilledRect(0, hardware::St7789Display::kHeight - 8, 8, 8, 0x001F);
    (void)display.drawFilledRect(hardware::St7789Display::kWidth - 8, hardware::St7789Display::kHeight - 8, 8, 8, 0xFFE0);
    std::array<uint16_t, hardware::St7789Display::kWidth> line{};
    line.fill(0xFFFF);
    (void)display.drawPixels(0, 0, hardware::St7789Display::kWidth, 1, line.data());
    (void)display.drawPixels(0, hardware::St7789Display::kHeight / 2, hardware::St7789Display::kWidth, 1, line.data());
    (void)display.drawRect(6, 44, 80, 34, 0xFFFF);
    (void)display.drawFilledRect(12, 54, 24, 14, 0x07E0);
    (void)display.drawLine(96, 44, 156, 78, 0xFFFF);
    (void)display.drawLine(156, 44, 96, 78, 0xFFFF);
    (void)display.drawText(8, 88, "FULL SMOKE", 0xFFFF, 0x0000);
    recordCheck(summary, CheckStatus::Pass, "display pattern", "color bars, shapes and text drawn");
}

[[maybe_unused]] void runI2CScan() {
    ESP_LOGI(TAG, "mode=i2c_scan");
    hardware::I2CBus bus;
    esp_err_t err = bus.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed: %s", esp_err_to_name(err));
        idleLoop("i2c_scan");
    }

    std::array<uint8_t, 128> addresses{};
    size_t count = 0;
    err = bus.scan(addresses, count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C scan failed: %s", esp_err_to_name(err));
        idleLoop("i2c_scan");
    }

    ESP_LOGI(TAG, "I2C devices found: %u", static_cast<unsigned>(count));
    for (size_t index = 0; index < count; ++index) {
        ESP_LOGI(TAG, " - 0x%02x", addresses[index]);
    }
    ESP_LOGI(TAG, "expected TCA8418 0x34: %s", containsAddress(addresses, count, hardware::Tca8418Keyboard::kAddress) ? "present" : "missing");
    ESP_LOGI(TAG, "expected BMI270  0x69: %s", containsAddress(addresses, count, hardware::Bmi270Imu::kAddress) ? "present" : "missing");
    ESP_LOGI(TAG, "candidate ES8311 0x18: %s", containsAddress(addresses, count, hardware::Es8311Codec::kDefaultAddress) ? "present" : "missing");
    idleLoop("i2c_scan");
}

[[maybe_unused]] void runKeyboardTest() {
    ESP_LOGI(TAG, "mode=keyboard_test");
    hardware::I2CBus bus;
    esp_err_t err = bus.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed: %s", esp_err_to_name(err));
        idleLoop("keyboard_test");
    }

    hardware::Tca8418Keyboard keyboard(bus);
    err = keyboard.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "TCA8418 init failed: %s", esp_err_to_name(err));
        ESP_LOGW(TAG, "Keyboard matrix/register mapping still needs hardware validation");
        idleLoop("keyboard_test");
    }

    while (true) {
        hardware::Tca8418Keyboard::KeyEvent event{};
        bool has_event = false;
        err = keyboard.readKeyEvent(event, has_event);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "keyboard read failed: %s", esp_err_to_name(err));
        } else if (has_event) {
            ESP_LOGI(TAG, "raw row=%u col=%u state=%s logical=0x%04x ascii='%c'",
                     event.raw.row, event.raw.column,
                     event.raw.state == hardware::Tca8418Keyboard::KeyState::Pressed ? "pressed" : "released",
                     event.logical_code,
                     event.ascii == '\0' ? ' ' : event.ascii);
            ESP_LOGI(TAG, "modifier/Fn state: not implemented yet");
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

[[maybe_unused]] void runDisplayTest() {
    ESP_LOGI(TAG, "mode=display_test");
    hardware::St7789Display display;
    esp_err_t err = display.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ST7789 init failed: %s", esp_err_to_name(err));
        idleLoop("display_test");
    }

    err = display.clear(0x0000);
    ESP_LOGI(TAG, "display clear result=%s", esp_err_to_name(err));

    constexpr uint16_t colors[] = {0xF800, 0x07E0, 0x001F, 0xFFE0, 0xFFFF, 0x0000};
    constexpr uint16_t bar_width = hardware::St7789Display::kWidth / (sizeof(colors) / sizeof(colors[0]));
    constexpr uint16_t bar_height = 36;
    for (size_t index = 0; index < sizeof(colors) / sizeof(colors[0]); ++index) {
        err = display.drawFilledRect(static_cast<uint16_t>(index * bar_width), 0, bar_width, bar_height, colors[index]);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "color bar draw failed: %s", esp_err_to_name(err));
            break;
        }
    }

    err = display.drawRect(0, 0, hardware::St7789Display::kWidth, hardware::St7789Display::kHeight, 0xFFFF);
    ESP_LOGI(TAG, "display draw full border result=%s", esp_err_to_name(err));
    (void)display.drawFilledRect(0, 0, 8, 8, 0xF800);
    (void)display.drawFilledRect(hardware::St7789Display::kWidth - 8, 0, 8, 8, 0x07E0);
    (void)display.drawFilledRect(0, hardware::St7789Display::kHeight - 8, 8, 8, 0x001F);
    (void)display.drawFilledRect(hardware::St7789Display::kWidth - 8, hardware::St7789Display::kHeight - 8, 8, 8, 0xFFE0);

    std::array<uint16_t, hardware::St7789Display::kWidth> line{};
    line.fill(0xFFFF);
    (void)display.drawPixels(0, 0, hardware::St7789Display::kWidth, 1, line.data());
    (void)display.drawPixels(0, hardware::St7789Display::kHeight / 2, hardware::St7789Display::kWidth, 1, line.data());
    err = display.drawRect(6, 44, 90, 36, 0xFFFF);
    ESP_LOGI(TAG, "display draw rect result=%s", esp_err_to_name(err));
    err = display.drawFilledRect(14, 56, 28, 14, 0x07E0);
    ESP_LOGI(TAG, "display draw filled rect result=%s", esp_err_to_name(err));
    err = display.drawLine(108, 44, 172, 80, 0xFFFF);
    ESP_LOGI(TAG, "display draw line A result=%s", esp_err_to_name(err));
    err = display.drawLine(172, 44, 108, 80, 0xFFFF);
    ESP_LOGI(TAG, "display draw line B result=%s", esp_err_to_name(err));
    err = display.drawText(8, 90, "CARDPUTER ADV", 0xFFFF, 0x0000);
    ESP_LOGI(TAG, "display draw text result=%s", esp_err_to_name(err));
    err = display.drawText(8, 106, "SHAPES + TEXT OK", 0xFFE0, 0x0000);
    ESP_LOGI(TAG, "display draw text 2 result=%s", esp_err_to_name(err));
    idleLoop("display_test");
}

[[maybe_unused]] void runAudioTest() {
    ESP_LOGI(TAG, "mode=audio_test");
    hardware::I2CBus bus;
    esp_err_t err = bus.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed: %s", esp_err_to_name(err));
        idleLoop("audio_test");
    }

    hardware::Es8311Codec codec(bus);
    err = codec.probe();
    ESP_LOGI(TAG, "ES8311 probe: %s", esp_err_to_name(err));
    err = codec.init(44100);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ES8311 init incomplete: %s", esp_err_to_name(err));
        ESP_LOGW(TAG, "Tone playback is skipped until the codec register sequence is implemented");
        idleLoop("audio_test");
    }

    hardware::I2SAudio audio;
    err = audio.init({.sample_rate_hz = 44100, .enable_input = false});
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2S init failed: %s", esp_err_to_name(err));
        idleLoop("audio_test");
    }

    std::array<int16_t, 4410> tone{};
    for (size_t index = 0; index < tone.size(); index += 2) {
        const float t = static_cast<float>(index / 2) / 44100.0f;
        const int16_t sample = static_cast<int16_t>(std::sin(2.0f * 3.14159265f * 440.0f * t) * 2000.0f);
        tone[index] = sample;
        tone[index + 1] = sample;
    }
    size_t written = 0;
    err = audio.write(tone.data(), tone.size(), written);
    ESP_LOGI(TAG, "tone write result=%s samples=%u", esp_err_to_name(err), static_cast<unsigned>(written));
    idleLoop("audio_test");
}

[[maybe_unused]] void runSdTest() {
    ESP_LOGI(TAG, "mode=sd_test");
    hardware::SdCard sd;
    esp_err_t err = sd.mount();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "microSD mount failed: %s", esp_err_to_name(err));
        idleLoop("sd_test");
    }

    DIR* dir = opendir("/sdcard");
    if (dir == nullptr) {
        ESP_LOGW(TAG, "could not open /sdcard");
    } else {
        while (dirent* entry = readdir(dir)) {
            ESP_LOGI(TAG, "root entry: %s", entry->d_name);
        }
        closedir(dir);
    }

#if APP_SMOKE_SD_WRITE
    const char* path = "/sdcard/cardputer_smoke.txt";
    FILE* file = fopen(path, "w");
    if (file != nullptr) {
        fputs("cardputer-adv smoke test\n", file);
        fclose(file);
    }
    char buffer[64]{};
    file = fopen(path, "r");
    if (file != nullptr) {
        fgets(buffer, sizeof(buffer), file);
        fclose(file);
        ESP_LOGI(TAG, "read back: %s", buffer);
    }
#else
    ESP_LOGI(TAG, "write/read test disabled; build with APP_SMOKE_SD_WRITE=1 to enable it");
#endif
    idleLoop("sd_test");
}

[[maybe_unused]] void runImuTest() {
    ESP_LOGI(TAG, "mode=imu_test");
    hardware::I2CBus bus;
    esp_err_t err = bus.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed: %s", esp_err_to_name(err));
        idleLoop("imu_test");
    }

    hardware::Bmi270Imu imu(bus);
    uint8_t chip_id = 0;
    err = imu.probe(chip_id);
    ESP_LOGI(TAG, "BMI270 probe result=%s chip_id=0x%02x", esp_err_to_name(err), chip_id);
    err = imu.init();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "BMI270 init incomplete: %s", esp_err_to_name(err));
        idleLoop("imu_test");
    }

    while (true) {
        hardware::Bmi270Imu::Sample sample{};
        err = imu.readSample(sample);
        ESP_LOGI(TAG, "imu read=%s accel=(%d,%d,%d) gyro=(%d,%d,%d)",
                 esp_err_to_name(err), sample.accel_x, sample.accel_y, sample.accel_z,
                 sample.gyro_x, sample.gyro_y, sample.gyro_z);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

[[maybe_unused]] void runBatteryTest() {
    ESP_LOGI(TAG, "mode=battery_test");
    hardware::BatteryMonitor battery;
    esp_err_t err = battery.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "battery ADC init failed: %s", esp_err_to_name(err));
        idleLoop("battery_test");
    }

    while (true) {
        int raw = 0;
        err = battery.readRaw(raw);
        ESP_LOGI(TAG, "battery raw result=%s value=%d", esp_err_to_name(err), raw);
        int millivolts = 0;
        err = battery.readApproxMillivolts(millivolts);
        ESP_LOGI(TAG, "battery estimated_mv result=%s value=%d", esp_err_to_name(err), millivolts);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

[[maybe_unused]] void runIrTest() {
    ESP_LOGI(TAG, "mode=ir_test");
    hardware::IrTransmitter ir;
    esp_err_t err = ir.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "IR init failed: %s", esp_err_to_name(err));
        idleLoop("ir_test");
    }

    const rmt_symbol_word_t pattern[] = {
        {.duration0 = 9000, .level0 = 1, .duration1 = 4500, .level1 = 0},
        {.duration0 = 560, .level0 = 1, .duration1 = 560, .level1 = 0},
        {.duration0 = 560, .level0 = 1, .duration1 = 1690, .level1 = 0},
    };
    err = ir.transmitRaw(pattern, sizeof(pattern) / sizeof(pattern[0]));
    ESP_LOGI(TAG, "IR transmit result=%s", esp_err_to_name(err));
    idleLoop("ir_test");
}

[[maybe_unused]] void runLvglDemo() {
    ESP_LOGI(TAG, "mode=lvgl_demo");
#if APP_ENABLE_LVGL
    ui::LvglPort lvgl;
    const esp_err_t err = lvgl.init();
    ESP_LOGW(TAG, "LVGL demo path is prepared but not implemented: %s", esp_err_to_name(err));
#else
    ESP_LOGW(TAG, "LVGL demo requires APP_ENABLE_LVGL=1");
#endif
    idleLoop("lvgl_demo");
}

[[maybe_unused]] void runWifiTest() {
    ESP_LOGI(TAG, "mode=wifi_test");
#if APP_ENABLE_WIFI
    services::WifiService wifi;
    esp_err_t err = wifi.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "WiFi init failed: %s", esp_err_to_name(err));
        idleLoop("wifi_test");
    }

    std::array<services::WifiService::AccessPoint, services::WifiService::kMaxScanResults> aps{};
    uint16_t count = aps.size();
    err = wifi.scan(aps.data(), count);
    ESP_LOGI(TAG, "WiFi scan result=%s count=%u", esp_err_to_name(err), static_cast<unsigned>(count));
    if (err == ESP_OK) {
        for (uint16_t index = 0; index < count; ++index) {
            ESP_LOGI(TAG, " - ssid='%s' rssi=%d channel=%u encrypted=%d",
                     aps[index].ssid, aps[index].rssi,
                     static_cast<unsigned>(aps[index].channel), aps[index].encrypted);
        }
    }

    if (std::strlen(APP_WIFI_SMOKE_SSID) > 0U) {
        ESP_LOGI(TAG, "Connecting to configured WiFi smoke-test SSID");
        err = wifi.connectStation(APP_WIFI_SMOKE_SSID, APP_WIFI_SMOKE_PASSWORD);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "WiFi connect start failed: %s", esp_err_to_name(err));
            idleLoop("wifi_test");
        }
        err = wifi.waitForConnection(15000);
        ESP_LOGI(TAG, "WiFi connection wait result=%s connected=%d", esp_err_to_name(err), wifi.isConnected());
    } else {
        ESP_LOGI(TAG, "No APP_WIFI_SMOKE_SSID provided; connect test skipped");
    }
#else
    ESP_LOGW(TAG, "WiFi test requires APP_ENABLE_WIFI=1");
#endif
    idleLoop("wifi_test");
}

[[maybe_unused]] void runFullSmokeTest() {
    ESP_LOGI(TAG, "mode=full_smoke_test");
    SmokeSummary summary{};

    esp_chip_info_t chip_info{};
    esp_chip_info(&chip_info);
    uint32_t flash_size = 0;
    esp_err_t err = esp_flash_get_size(nullptr, &flash_size);
    recordErr(summary, CheckStatus::Pass, "flash info", err);
    ESP_LOGI(TAG, "chip cores=%d features=0x%lx revision=%d flash=%lu bytes reset=%s",
             chip_info.cores, static_cast<unsigned long>(chip_info.features), chip_info.revision,
             static_cast<unsigned long>(flash_size), resetReasonName(esp_reset_reason()));

    hardware::I2CBus bus;
    err = bus.init();
    recordErr(summary, CheckStatus::Pass, "I2C init", err);

    std::array<uint8_t, 128> addresses{};
    size_t address_count = 0;
    if (err == ESP_OK) {
        err = bus.scan(addresses, address_count);
        recordErr(summary, CheckStatus::Pass, "I2C scan", err);
        ESP_LOGI(TAG, "I2C devices found=%u", static_cast<unsigned>(address_count));
        for (size_t index = 0; index < address_count; ++index) {
            ESP_LOGI(TAG, " - 0x%02x", addresses[index]);
        }
        recordCheck(summary,
                    containsAddress(addresses, address_count, hardware::Tca8418Keyboard::kAddress) ? CheckStatus::Pass : CheckStatus::Warn,
                    "TCA8418 address", "expected 0x34");
        recordCheck(summary,
                    containsAddress(addresses, address_count, hardware::Bmi270Imu::kAddress) ? CheckStatus::Pass : CheckStatus::Warn,
                    "BMI270 address", "expected 0x69");
        recordCheck(summary,
                    containsAddress(addresses, address_count, hardware::Es8311Codec::kDefaultAddress) ? CheckStatus::Pass : CheckStatus::Warn,
                    "ES8311 address", "candidate 0x18");
    }

    if (bus.isInitialized()) {
        hardware::Tca8418Keyboard keyboard(bus);
        err = keyboard.init();
        recordErr(summary, CheckStatus::Pass, "keyboard init", err);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Press keyboard buttons now; listening for %u ms",
                     static_cast<unsigned>(APP_FULL_SMOKE_KEYBOARD_MS));
            uint16_t key_events = 0;
            const TickType_t end_tick = xTaskGetTickCount() + pdMS_TO_TICKS(APP_FULL_SMOKE_KEYBOARD_MS);
            while (xTaskGetTickCount() < end_tick) {
                hardware::Tca8418Keyboard::KeyEvent event{};
                bool has_event = false;
                err = keyboard.readKeyEvent(event, has_event);
                if (err != ESP_OK) {
                    ESP_LOGW(TAG, "keyboard read failed: %s", esp_err_to_name(err));
                    break;
                }
                if (has_event) {
                    ++key_events;
                    ESP_LOGI(TAG, "key event row=%u col=%u state=%s logical=0x%04x ascii='%c'",
                             event.raw.row, event.raw.column,
                             event.raw.state == hardware::Tca8418Keyboard::KeyState::Pressed ? "pressed" : "released",
                             event.logical_code, event.ascii == '\0' ? ' ' : event.ascii);
                }
                vTaskDelay(pdMS_TO_TICKS(20));
            }
            char detail[48]{};
            std::snprintf(detail, sizeof(detail), "%u events", static_cast<unsigned>(key_events));
            recordCheck(summary, key_events > 0 ? CheckStatus::Pass : CheckStatus::Warn, "keyboard interaction", detail);
        }

        hardware::Bmi270Imu imu(bus);
        uint8_t chip_id = 0;
        err = imu.probe(chip_id);
        char imu_detail[48]{};
        std::snprintf(imu_detail, sizeof(imu_detail), "%s chip_id=0x%02x", esp_err_to_name(err), chip_id);
        recordCheck(summary, err == ESP_OK ? CheckStatus::Pass : CheckStatus::Warn, "BMI270 probe", imu_detail);
        err = imu.init();
        recordCheck(summary, err == ESP_ERR_NOT_SUPPORTED ? CheckStatus::Warn : (err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail),
                    "BMI270 init", esp_err_to_name(err));

        hardware::Es8311Codec codec(bus);
        err = codec.probe();
        recordCheck(summary, err == ESP_OK ? CheckStatus::Pass : CheckStatus::Warn, "ES8311 probe", esp_err_to_name(err));
        err = codec.init(44100);
        recordCheck(summary, err == ESP_ERR_NOT_SUPPORTED ? CheckStatus::Warn : (err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail),
                    "ES8311 init", esp_err_to_name(err));
    }

    hardware::St7789Display display;
    err = display.init();
    recordErr(summary, CheckStatus::Pass, "display init", err);
    if (err == ESP_OK) {
        drawDisplayPattern(display, summary);
        err = display.setBacklight(true);
        recordErr(summary, CheckStatus::Pass, "display backlight", err);
    }

    hardware::BatteryMonitor battery;
    err = battery.init();
    recordErr(summary, CheckStatus::Pass, "battery ADC init", err);
    if (err == ESP_OK) {
        int raw = 0;
        err = battery.readRaw(raw);
        char detail[48]{};
        std::snprintf(detail, sizeof(detail), "%s raw=%d", esp_err_to_name(err), raw);
        recordCheck(summary, err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail, "battery raw", detail);
        int millivolts = 0;
        err = battery.readApproxMillivolts(millivolts);
        std::snprintf(detail, sizeof(detail), "%s approx_mv=%d", esp_err_to_name(err), millivolts);
        recordCheck(summary, err == ESP_ERR_NOT_SUPPORTED ? CheckStatus::Warn : (err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail),
                    "battery voltage", detail);
    }

    hardware::IrTransmitter ir;
    err = ir.init();
    recordErr(summary, CheckStatus::Pass, "IR init", err);
    if (err == ESP_OK) {
        const rmt_symbol_word_t pattern[] = {
            {.duration0 = 9000, .level0 = 1, .duration1 = 4500, .level1 = 0},
            {.duration0 = 560, .level0 = 1, .duration1 = 560, .level1 = 0},
            {.duration0 = 560, .level0 = 1, .duration1 = 1690, .level1 = 0},
        };
        err = ir.transmitRaw(pattern, sizeof(pattern) / sizeof(pattern[0]));
        recordErr(summary, CheckStatus::Pass, "IR transmit", err);
    }

    hardware::I2SAudio i2s;
    err = i2s.init({.sample_rate_hz = 44100, .enable_input = false});
    recordCheck(summary, err == ESP_OK ? CheckStatus::Pass : CheckStatus::Warn, "I2S init", esp_err_to_name(err));
    if (err == ESP_OK) {
        (void)i2s.stop();
    }

    hardware::SdCard sd;
    err = sd.mount();
    recordCheck(summary, err == ESP_OK ? CheckStatus::Pass : CheckStatus::Warn, "microSD mount", esp_err_to_name(err));
    if (err == ESP_OK) {
        DIR* dir = opendir("/sdcard");
        if (dir != nullptr) {
            uint8_t entries = 0;
            while (entries < 8) {
                dirent* entry = readdir(dir);
                if (entry == nullptr) {
                    break;
                }
                ESP_LOGI(TAG, "sd root entry: %s", entry->d_name);
                ++entries;
            }
            closedir(dir);
            recordCheck(summary, CheckStatus::Pass, "microSD list", "root directory opened");
        } else {
            recordCheck(summary, CheckStatus::Warn, "microSD list", "could not open /sdcard");
        }
        (void)sd.unmount();
    }

#if APP_ENABLE_WIFI
    services::WifiService wifi;
    err = wifi.init();
    recordErr(summary, CheckStatus::Pass, "WiFi init", err);
    if (err == ESP_OK) {
        std::array<services::WifiService::AccessPoint, services::WifiService::kMaxScanResults> aps{};
        uint16_t count = aps.size();
        err = wifi.scan(aps.data(), count);
        char detail[48]{};
        std::snprintf(detail, sizeof(detail), "%s count=%u", esp_err_to_name(err), static_cast<unsigned>(count));
        recordCheck(summary, err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail, "WiFi scan", detail);
        for (uint16_t index = 0; err == ESP_OK && index < count; ++index) {
            ESP_LOGI(TAG, "wifi ap ssid='%s' rssi=%d channel=%u encrypted=%d",
                     aps[index].ssid, aps[index].rssi,
                     static_cast<unsigned>(aps[index].channel), aps[index].encrypted);
        }

        if (std::strlen(APP_WIFI_SMOKE_SSID) > 0U) {
            err = wifi.connectStation(APP_WIFI_SMOKE_SSID, APP_WIFI_SMOKE_PASSWORD);
            recordErr(summary, CheckStatus::Pass, "WiFi connect start", err);
            if (err == ESP_OK) {
                err = wifi.waitForConnection(15000);
                recordCheck(summary, err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail,
                            "WiFi connection", esp_err_to_name(err));
            }
        } else {
            recordCheck(summary, CheckStatus::Warn, "WiFi connection", "APP_WIFI_SMOKE_SSID not provided");
        }
        (void)wifi.disconnect();
        (void)wifi.stop();
    }
#else
    recordCheck(summary, CheckStatus::Warn, "WiFi", "APP_ENABLE_WIFI=0");
#endif

    ui::LvglPort lvgl;
    err = lvgl.init();
    recordCheck(summary, err == ESP_ERR_NOT_SUPPORTED ? CheckStatus::Warn : (err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail),
                "LVGL hook", esp_err_to_name(err));

    services::BleService ble;
    err = ble.init();
    recordCheck(summary, err == ESP_ERR_NOT_SUPPORTED ? CheckStatus::Warn : (err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail),
                "BLE hook", esp_err_to_name(err));

    services::UsbService usb;
    err = usb.init();
    recordCheck(summary, err == ESP_ERR_NOT_SUPPORTED ? CheckStatus::Warn : (err == ESP_OK ? CheckStatus::Pass : CheckStatus::Fail),
                "USB hook", esp_err_to_name(err));

    ESP_LOGI(TAG, "FULL_SMOKE_SUMMARY pass=%u warn=%u fail=%u",
             static_cast<unsigned>(summary.passed),
             static_cast<unsigned>(summary.warnings),
             static_cast<unsigned>(summary.failed));
    if (summary.failed == 0) {
        ESP_LOGI(TAG, "FULL_SMOKE_RESULT PASS_WITH_WARNINGS_ALLOWED");
    } else {
        ESP_LOGE(TAG, "FULL_SMOKE_RESULT FAIL");
    }
    idleLoop("full_smoke_test");
}
}  // namespace

void runSelectedMode() {
#if APP_MODE_I2C_SCAN
    runI2CScan();
#elif APP_MODE_KEYBOARD_TEST
    runKeyboardTest();
#elif APP_MODE_DISPLAY_TEST
    runDisplayTest();
#elif APP_MODE_AUDIO_TEST
    runAudioTest();
#elif APP_MODE_SD_TEST
    runSdTest();
#elif APP_MODE_IMU_TEST
    runImuTest();
#elif APP_MODE_BATTERY_TEST
    runBatteryTest();
#elif APP_MODE_IR_TEST
    runIrTest();
#elif APP_MODE_LVGL_DEMO
    runLvglDemo();
#elif APP_MODE_WIFI_TEST
    runWifiTest();
#elif APP_MODE_FULL_SMOKE_TEST
    runFullSmokeTest();
#else
    runBootInfo();
#endif
}

}  // namespace cardputer::app::smoke_tests
