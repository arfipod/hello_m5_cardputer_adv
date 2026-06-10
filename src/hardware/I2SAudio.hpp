#pragma once

#include <cstddef>
#include <cstdint>

#include "driver/i2s_std.h"
#include "esp_err.h"

namespace cardputer::hardware {

class I2SAudio {
public:
    struct Config {
        uint32_t sample_rate_hz = 44100;
        bool enable_input = false;
    };

    I2SAudio() = default;
    ~I2SAudio();

    I2SAudio(const I2SAudio&) = delete;
    I2SAudio& operator=(const I2SAudio&) = delete;

    esp_err_t init();
    esp_err_t init(const Config& config);
    esp_err_t write(const int16_t* samples, size_t sample_count, size_t& samples_written);
    esp_err_t read(int16_t* samples, size_t sample_count, size_t& samples_read);
    esp_err_t stop();

private:
    i2s_chan_handle_t tx_handle_ = nullptr;
    i2s_chan_handle_t rx_handle_ = nullptr;
    bool initialized_ = false;
};

}  // namespace cardputer::hardware
