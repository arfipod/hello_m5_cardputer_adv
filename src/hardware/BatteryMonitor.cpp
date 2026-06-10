#include "hardware/BatteryMonitor.hpp"

#include "esp_adc/adc_cali_scheme.h"
#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {
namespace {
constexpr adc_atten_t kBatteryAdcAttenuation = ADC_ATTEN_DB_12;
constexpr adc_bitwidth_t kBatteryAdcBitwidth = ADC_BITWIDTH_DEFAULT;
constexpr int kBatteryVoltageRatioNumerator = 2;
constexpr int kBatteryVoltageRatioDenominator = 1;
}

BatteryMonitor::~BatteryMonitor() {
    if (calibration_handle_ != nullptr) {
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
        if (calibration_scheme_ == ADC_CALI_SCHEME_VER_CURVE_FITTING) {
            adc_cali_delete_scheme_curve_fitting(calibration_handle_);
        }
#endif
#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
        if (calibration_scheme_ == ADC_CALI_SCHEME_VER_LINE_FITTING) {
            adc_cali_delete_scheme_line_fitting(calibration_handle_);
        }
#endif
    }
    if (unit_handle_ != nullptr) {
        adc_oneshot_del_unit(unit_handle_);
    }
}

esp_err_t BatteryMonitor::initCalibration() {
    if (calibration_handle_ != nullptr) {
        return ESP_OK;
    }

    adc_cali_scheme_ver_t scheme_mask = {};
    esp_err_t err = adc_cali_check_scheme(&scheme_mask);
    if (err != ESP_OK) {
        return err;
    }

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if ((scheme_mask & ADC_CALI_SCHEME_VER_CURVE_FITTING) != 0) {
        const adc_cali_curve_fitting_config_t calibration_config = {
            .unit_id = unit_,
            .chan = channel_,
            .atten = kBatteryAdcAttenuation,
            .bitwidth = kBatteryAdcBitwidth,
        };
        err = adc_cali_create_scheme_curve_fitting(&calibration_config, &calibration_handle_);
        if (err == ESP_OK) {
            calibration_scheme_ = ADC_CALI_SCHEME_VER_CURVE_FITTING;
            return ESP_OK;
        }
        if (err != ESP_ERR_NOT_SUPPORTED) {
            return err;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if ((scheme_mask & ADC_CALI_SCHEME_VER_LINE_FITTING) != 0) {
        const adc_cali_line_fitting_config_t calibration_config = {
            .unit_id = unit_,
            .atten = kBatteryAdcAttenuation,
            .bitwidth = kBatteryAdcBitwidth,
        };
        err = adc_cali_create_scheme_line_fitting(&calibration_config, &calibration_handle_);
        if (err == ESP_OK) {
            calibration_scheme_ = ADC_CALI_SCHEME_VER_LINE_FITTING;
            return ESP_OK;
        }
        return err;
    }
#endif

    return ESP_ERR_NOT_SUPPORTED;
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
        .atten = kBatteryAdcAttenuation,
        .bitwidth = kBatteryAdcBitwidth,
    };
    err = adc_oneshot_config_channel(unit_handle_, channel_, &channel_config);
    if (err == ESP_OK) {
        (void)initCalibration();
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
    if (calibration_handle_ == nullptr) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    int adc_millivolts = 0;
    err = adc_cali_raw_to_voltage(calibration_handle_, raw, &adc_millivolts);
    if (err != ESP_OK) {
        return err;
    }

    millivolts = (adc_millivolts * kBatteryVoltageRatioNumerator) / kBatteryVoltageRatioDenominator;
    return ESP_OK;
}

}  // namespace cardputer::hardware
