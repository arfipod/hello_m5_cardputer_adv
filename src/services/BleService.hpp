#pragma once

#include "esp_err.h"

namespace cardputer::services {

class BleService {
public:
    esp_err_t init();
    esp_err_t stop();
};

}  // namespace cardputer::services
