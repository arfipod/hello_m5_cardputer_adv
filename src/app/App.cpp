#include "app/App.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hardware/BoardPins.hpp"

namespace cardputer::app {
namespace {
constexpr const char* TAG = "app";
}

void App::run() {
    ESP_LOGI(TAG, "Generic App started");
    ESP_LOGI(TAG, "I2C pins: SDA=GPIO%d SCL=GPIO%d",
             static_cast<int>(hardware::pins::I2C_SDA),
             static_cast<int>(hardware::pins::I2C_SCL));
    ESP_LOGI(TAG, "Subsystem flags display=%d keyboard=%d audio=%d imu=%d sd=%d battery=%d ir=%d lvgl=%d usb=%d",
             APP_ENABLE_DISPLAY, APP_ENABLE_KEYBOARD, APP_ENABLE_AUDIO, APP_ENABLE_IMU,
             APP_ENABLE_SD, APP_ENABLE_BATTERY, APP_ENABLE_IR, APP_ENABLE_LVGL, APP_ENABLE_USB);

    uint32_t heartbeat = 0;
    while (true) {
        ESP_LOGI(TAG, "heartbeat=%lu", static_cast<unsigned long>(heartbeat++));
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

}  // namespace cardputer::app
