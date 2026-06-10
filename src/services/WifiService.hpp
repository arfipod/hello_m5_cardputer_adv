#pragma once

#include <cstdint>

#include "esp_err.h"

namespace cardputer::services {

class WifiService {
public:
    static constexpr uint16_t kMaxScanResults = 16;

    struct AccessPoint {
        char ssid[33] = {};
        int8_t rssi = 0;
        uint8_t channel = 0;
        bool encrypted = false;
    };

    esp_err_t init();
    esp_err_t startStation();
    esp_err_t connectStation(const char* ssid, const char* password);
    esp_err_t waitForConnection(uint32_t timeout_ms);
    bool isConnected() const;
    esp_err_t disconnect();
    esp_err_t scan(AccessPoint* results, uint16_t& count);
    esp_err_t stop();
};

}  // namespace cardputer::services
