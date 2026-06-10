#pragma once

#include "esp_err.h"

namespace cardputer::services {

class WifiService {
public:
    esp_err_t init();
    esp_err_t stop();
};

}  // namespace cardputer::services
