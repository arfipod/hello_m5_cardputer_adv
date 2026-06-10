#include "hardware/Es8311Codec.hpp"

#include "esp_log.h"

namespace cardputer::hardware {
namespace {
constexpr const char* TAG = "es8311";
}

Es8311Codec::Es8311Codec(I2CBus& bus, uint8_t address) : bus_(bus), address_(address) {}

esp_err_t Es8311Codec::probe() {
    if (!bus_.isInitialized()) {
        return ESP_ERR_INVALID_STATE;
    }
    uint8_t value = 0;
    return bus_.readRegister(address_, 0x00, &value, 1);
}

esp_err_t Es8311Codec::init(uint32_t sample_rate_hz) {
    (void)sample_rate_hz;
    ESP_LOGW(TAG, "ES8311 register sequence is not implemented yet");
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t Es8311Codec::configureSampleRate(uint32_t sample_rate_hz) {
    (void)sample_rate_hz;
    return initialized_ ? ESP_ERR_NOT_SUPPORTED : ESP_ERR_INVALID_STATE;
}

esp_err_t Es8311Codec::setVolume(uint8_t percent) {
    if (percent > 100) {
        return ESP_ERR_INVALID_ARG;
    }
    return initialized_ ? ESP_ERR_NOT_SUPPORTED : ESP_ERR_INVALID_STATE;
}

esp_err_t Es8311Codec::setMuted(bool muted) {
    (void)muted;
    return initialized_ ? ESP_ERR_NOT_SUPPORTED : ESP_ERR_INVALID_STATE;
}

}  // namespace cardputer::hardware
