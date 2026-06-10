#include "ui/LvglPort.hpp"

namespace cardputer::ui {

esp_err_t LvglPort::init() {
#if APP_ENABLE_LVGL
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t LvglPort::tick() {
#if APP_ENABLE_LVGL
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_ERR_INVALID_STATE;
#endif
}

}  // namespace cardputer::ui
