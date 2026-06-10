#pragma once

#include <cstdint>

#include "esp_err.h"

namespace cardputer::services {

class BleService {
public:
    struct AdvertisingConfig {
        const char* device_name = "cardputer-adv";
        uint16_t appearance = 0;
    };

    esp_err_t init();
    esp_err_t startAdvertising();
    esp_err_t startAdvertising(const AdvertisingConfig& config);
    esp_err_t stopAdvertising();
    esp_err_t stop();
};

}  // namespace cardputer::services
