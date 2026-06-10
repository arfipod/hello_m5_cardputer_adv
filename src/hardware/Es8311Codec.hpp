#pragma once

#include <cstdint>

#include "esp_err.h"
#include "hardware/I2CBus.hpp"

namespace cardputer::hardware {

class Es8311Codec {
public:
    static constexpr uint8_t kDefaultAddress = 0x18;

    explicit Es8311Codec(I2CBus& bus, uint8_t address = kDefaultAddress);

    esp_err_t probe();
    esp_err_t init(uint32_t sample_rate_hz);
    esp_err_t configureSampleRate(uint32_t sample_rate_hz);
    esp_err_t setVolume(uint8_t percent);
    esp_err_t setMuted(bool muted);

private:
    I2CBus& bus_;
    uint8_t address_;
    bool initialized_ = false;
};

}  // namespace cardputer::hardware
