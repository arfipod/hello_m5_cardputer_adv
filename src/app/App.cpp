#include "app/App.hpp"

#include "apps/smoke_tests/SmokeTests.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "hardware/Bmi270Imu.hpp"
#include "hardware/BoardPins.hpp"
#include "hardware/Es8311Codec.hpp"
#include "hardware/I2CBus.hpp"
#include "services/BleService.hpp"
#include "services/UsbService.hpp"
#include "services/WifiService.hpp"
#include "ui/LvglPort.hpp"

namespace cardputer::app {
namespace {
constexpr const char* TAG = "app";

void logHookResult(const char* name, esp_err_t err) {
    if (err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "%s pending: %s", name, esp_err_to_name(err));
    } else if (err != ESP_OK) {
        ESP_LOGW(TAG, "%s returned: %s", name, esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "%s ready", name);
    }
}

void reportPendingBringupHooks() {
    hardware::I2CBus i2c_bus;
    hardware::Es8311Codec codec(i2c_bus);
    hardware::Bmi270Imu imu(i2c_bus);
    ui::LvglPort lvgl;
    services::WifiService wifi;
    services::BleService ble;
    services::UsbService usb;

    logHookResult("ES8311 codec init hook", codec.init(44100));
    logHookResult("BMI270 IMU init hook", imu.init());
    logHookResult("LVGL init hook", lvgl.init());
    logHookResult("WiFi service init hook", wifi.init());
    logHookResult("BLE service init hook", ble.init());
    logHookResult("USB service init hook", usb.init());
}
}

void App::run() {
    ESP_LOGI(TAG, "Generic App started");
    ESP_LOGI(TAG, "I2C pins: SDA=GPIO%d SCL=GPIO%d",
             static_cast<int>(hardware::pins::I2C_SDA),
             static_cast<int>(hardware::pins::I2C_SCL));
    ESP_LOGI(TAG, "Subsystem flags display=%d keyboard=%d audio=%d imu=%d sd=%d battery=%d ir=%d lvgl=%d wifi=%d ble=%d usb=%d",
             APP_ENABLE_DISPLAY, APP_ENABLE_KEYBOARD, APP_ENABLE_AUDIO, APP_ENABLE_IMU,
             APP_ENABLE_SD, APP_ENABLE_BATTERY, APP_ENABLE_IR, APP_ENABLE_LVGL,
             APP_ENABLE_WIFI, APP_ENABLE_BLE, APP_ENABLE_USB);
    reportPendingBringupHooks();

    smoke_tests::runSelectedMode();
}

}  // namespace cardputer::app
