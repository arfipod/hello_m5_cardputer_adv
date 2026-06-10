#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

namespace cardputer::hardware {

class BatteryMonitor {
public:
    BatteryMonitor() = default;
    ~BatteryMonitor();

    BatteryMonitor(const BatteryMonitor&) = delete;
    BatteryMonitor& operator=(const BatteryMonitor&) = delete;

    esp_err_t init();
    esp_err_t readRaw(int& raw);
    esp_err_t readApproxMillivolts(int& millivolts);

private:
    adc_oneshot_unit_handle_t unit_handle_ = nullptr;
    adc_unit_t unit_ = ADC_UNIT_1;
    adc_channel_t channel_ = ADC_CHANNEL_0;
    bool initialized_ = false;
};

}  // namespace cardputer::hardware
