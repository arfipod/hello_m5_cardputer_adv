#include "hardware/Bmi270Imu.hpp"

#include "esp_log.h"

namespace cardputer::hardware {
namespace {
constexpr const char* TAG = "bmi270";
constexpr uint8_t REG_CHIP_ID = 0x00;
}

Bmi270Imu::Bmi270Imu(I2CBus& bus) : bus_(bus) {}

esp_err_t Bmi270Imu::probe(uint8_t& chip_id) {
    if (!bus_.isInitialized()) {
        return ESP_ERR_INVALID_STATE;
    }
    return bus_.readRegister(kAddress, REG_CHIP_ID, &chip_id, 1);
}

esp_err_t Bmi270Imu::init() {
    ESP_LOGW(TAG, "BMI270 firmware upload and configuration sequence is not implemented yet");
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t Bmi270Imu::readSample(Sample& sample) {
    (void)sample;
    return initialized_ ? ESP_ERR_NOT_SUPPORTED : ESP_ERR_INVALID_STATE;
}

}  // namespace cardputer::hardware
