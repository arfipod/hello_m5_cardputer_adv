#include "services/BleService.hpp"

namespace cardputer::services {

esp_err_t BleService::init() {
#if APP_ENABLE_BLE
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t BleService::stop() {
    return ESP_OK;
}

}  // namespace cardputer::services
