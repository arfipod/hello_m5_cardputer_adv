#include "hardware/BatteryMonitor.hpp"

#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

BatteryMonitor::~BatteryMonitor() {
    if (unit_handle_ != nullptr) {
        adc_oneshot_del_unit(unit_handle_);
    }
}

esp_err_t BatteryMonitor::init() {
    if (initialized_) {
        return ESP_OK;
    }

    esp_err_t err = adc_oneshot_io_to_channel(static_cast<int>(pins::BATTERY_ADC), &unit_, &channel_);
    if (err != ESP_OK) {
        return err;
    }

    const adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = unit_,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    err = adc_oneshot_new_unit(&unit_config, &unit_handle_);
    if (err != ESP_OK) {
        return err;
    }

    const adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    err = adc_oneshot_config_channel(unit_handle_, channel_, &channel_config);
    if (err == ESP_OK) {
        initialized_ = true;
    }
    return err;
}

esp_err_t BatteryMonitor::readRaw(int& raw) {
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }
    return adc_oneshot_read(unit_handle_, channel_, &raw);
}

esp_err_t BatteryMonitor::readApproxMillivolts(int& millivolts) {
    int raw = 0;
    esp_err_t err = readRaw(raw);
    if (err != ESP_OK) {
        return err;
    }
    // TODO: replace this rough conversion with calibrated ADC and divider values.
    millivolts = raw;
    return ESP_ERR_NOT_SUPPORTED;
}

}  // namespace cardputer::hardware
