#pragma once

#include <cstddef>
#include <cstdint>

#include "esp_err.h"

namespace cardputer::services {

class UsbService {
public:
    enum class DeviceClass : uint8_t {
        CdcAcm,
        Hid,
        Midi,
        Composite,
    };

    esp_err_t init();
    esp_err_t startDevice(DeviceClass device_class);
    esp_err_t write(const uint8_t* data, size_t length, size_t& written);
    esp_err_t stop();
};

}  // namespace cardputer::services
