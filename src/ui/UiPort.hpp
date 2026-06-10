#pragma once

#include "esp_err.h"

namespace cardputer::ui {

class UiPort {
public:
    esp_err_t init();
    esp_err_t tick();
};

}  // namespace cardputer::ui
