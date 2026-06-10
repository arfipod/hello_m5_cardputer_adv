#pragma once

#include <cstdint>

#include "esp_err.h"
#include "hardware/I2CBus.hpp"

namespace cardputer::hardware {

class Bmi270Imu {
public:
    static constexpr uint8_t kAddress = 0x69;

    struct Sample {
        int16_t accel_x = 0;
        int16_t accel_y = 0;
        int16_t accel_z = 0;
        int16_t gyro_x = 0;
        int16_t gyro_y = 0;
        int16_t gyro_z = 0;
    };

    explicit Bmi270Imu(I2CBus& bus);

    esp_err_t probe(uint8_t& chip_id);
    esp_err_t init();
    esp_err_t readSample(Sample& sample);

private:
    I2CBus& bus_;
    bool initialized_ = false;
};

}  // namespace cardputer::hardware
