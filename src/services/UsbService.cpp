#include "services/UsbService.hpp"

namespace cardputer::services {

esp_err_t UsbService::init() {
#if APP_ENABLE_USB
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t UsbService::stop() {
    return ESP_OK;
}

}  // namespace cardputer::services
