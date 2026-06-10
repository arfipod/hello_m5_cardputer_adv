#include "services/UsbService.hpp"

#include "esp_log.h"

namespace cardputer::services {
namespace {
constexpr const char* TAG = "usb_service";
}

esp_err_t UsbService::init() {
#if APP_ENABLE_USB
    ESP_LOGW(TAG, "USB hook enabled, but TinyUSB descriptors/device bring-up are not implemented yet");
    return ESP_ERR_NOT_SUPPORTED;
#else
    ESP_LOGW(TAG, "USB hook present but disabled; no bring-up sequence is linked");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t UsbService::startDevice(DeviceClass device_class) {
    (void)device_class;
#if APP_ENABLE_USB
    ESP_LOGW(TAG, "USB device start requires TinyUSB descriptors and class-specific callbacks");
#endif
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t UsbService::write(const uint8_t* data, size_t length, size_t& written) {
    written = 0;
    if (length > 0U && data == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
#if APP_ENABLE_USB
    ESP_LOGW(TAG, "USB write requires a concrete USB class endpoint");
#endif
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t UsbService::stop() {
    return ESP_OK;
}

}  // namespace cardputer::services
