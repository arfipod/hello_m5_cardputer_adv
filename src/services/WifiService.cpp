#include "services/WifiService.hpp"

namespace cardputer::services {

esp_err_t WifiService::init() {
#if APP_ENABLE_WIFI
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t WifiService::stop() {
    return ESP_OK;
}

}  // namespace cardputer::services
