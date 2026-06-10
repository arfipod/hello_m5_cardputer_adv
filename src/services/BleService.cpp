#include "services/BleService.hpp"

#include "esp_log.h"

namespace cardputer::services {
namespace {
constexpr const char* TAG = "ble_service";
}

esp_err_t BleService::init() {
#if APP_ENABLE_BLE
    ESP_LOGW(TAG, "BLE hook enabled, but controller/host/GATT bring-up is not implemented yet");
    return ESP_ERR_NOT_SUPPORTED;
#else
    ESP_LOGW(TAG, "BLE hook present but disabled; no bring-up sequence is linked");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t BleService::startAdvertising() {
    return startAdvertising(AdvertisingConfig{});
}

esp_err_t BleService::startAdvertising(const AdvertisingConfig& config) {
    (void)config;
#if APP_ENABLE_BLE
    ESP_LOGW(TAG, "BLE advertising requires a selected host stack and GAP/GATT setup");
#endif
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t BleService::stopAdvertising() {
#if APP_ENABLE_BLE
    ESP_LOGW(TAG, "BLE advertising stop is unavailable until BLE bring-up is implemented");
    return ESP_ERR_NOT_SUPPORTED;
#else
    return ESP_OK;
#endif
}

esp_err_t BleService::stop() {
    return ESP_OK;
}

}  // namespace cardputer::services
