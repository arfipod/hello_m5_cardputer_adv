#pragma once

#include <cstddef>

#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "esp_err.h"

namespace cardputer::hardware {

class IrTransmitter {
public:
    struct Config {
        uint32_t resolution_hz = 1000000;
        uint32_t carrier_hz = 38000;
    };

    IrTransmitter() = default;
    ~IrTransmitter();

    IrTransmitter(const IrTransmitter&) = delete;
    IrTransmitter& operator=(const IrTransmitter&) = delete;

    esp_err_t init();
    esp_err_t init(const Config& config);
    esp_err_t transmitRaw(const rmt_symbol_word_t* symbols, size_t symbol_count);

private:
    rmt_channel_handle_t channel_ = nullptr;
    rmt_encoder_handle_t copy_encoder_ = nullptr;
    bool initialized_ = false;
};

}  // namespace cardputer::hardware
