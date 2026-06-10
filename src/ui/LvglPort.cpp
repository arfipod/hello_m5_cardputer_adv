#include "ui/LvglPort.hpp"

#include "esp_log.h"

namespace cardputer::ui {
namespace {
constexpr const char* TAG = "lvgl_port";
}

esp_err_t LvglPort::init() {
#if APP_ENABLE_LVGL
    ESP_LOGW(TAG, "LVGL hook enabled, but dependency setup/display flush/input bring-up are not implemented yet");
    return ESP_ERR_NOT_SUPPORTED;
#else
    ESP_LOGW(TAG, "LVGL hook present but disabled; no UI bring-up sequence is linked");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t LvglPort::tick() {
#if APP_ENABLE_LVGL
    ESP_LOGW(TAG, "LVGL tick hook is unavailable until lv_timer_handler scheduling is implemented");
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_ERR_INVALID_STATE;
#endif
}

}  // namespace cardputer::ui
