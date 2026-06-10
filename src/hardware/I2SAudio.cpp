#include "hardware/I2SAudio.hpp"

#include "driver/i2s_common.h"
#include "freertos/FreeRTOS.h"
#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

I2SAudio::~I2SAudio() {
    stop();
    if (tx_handle_ != nullptr) {
        i2s_del_channel(tx_handle_);
    }
    if (rx_handle_ != nullptr) {
        i2s_del_channel(rx_handle_);
    }
}

esp_err_t I2SAudio::init() {
    return init(Config{});
}

esp_err_t I2SAudio::init(const Config& config) {
    if (initialized_) {
        return ESP_OK;
    }

    i2s_chan_config_t channel_config = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    esp_err_t err = i2s_new_channel(&channel_config, &tx_handle_, config.enable_input ? &rx_handle_ : nullptr);
    if (err != ESP_OK) {
        return err;
    }

    i2s_std_config_t std_config = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(config.sample_rate_hz),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = pins::I2S_BCLK,
            .ws = pins::I2S_LRCLK,
            .dout = pins::I2S_DOUT_TO_CODEC_DSDIN,
            .din = config.enable_input ? pins::I2S_DIN_FROM_CODEC_ASDOUT : I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    err = i2s_channel_init_std_mode(tx_handle_, &std_config);
    if (err != ESP_OK) {
        return err;
    }
    err = i2s_channel_enable(tx_handle_);
    if (err != ESP_OK) {
        return err;
    }

    if (rx_handle_ != nullptr) {
        err = i2s_channel_init_std_mode(rx_handle_, &std_config);
        if (err != ESP_OK) {
            return err;
        }
        err = i2s_channel_enable(rx_handle_);
        if (err != ESP_OK) {
            return err;
        }
    }

    initialized_ = true;
    return ESP_OK;
}

esp_err_t I2SAudio::write(const int16_t* samples, size_t sample_count, size_t& samples_written) {
    samples_written = 0;
    if (!initialized_ || tx_handle_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (samples == nullptr && sample_count > 0) {
        return ESP_ERR_INVALID_ARG;
    }
    size_t bytes_written = 0;
    esp_err_t err = i2s_channel_write(tx_handle_, samples, sample_count * sizeof(int16_t), &bytes_written, pdMS_TO_TICKS(100));
    samples_written = bytes_written / sizeof(int16_t);
    return err;
}

esp_err_t I2SAudio::read(int16_t* samples, size_t sample_count, size_t& samples_read) {
    samples_read = 0;
    if (!initialized_ || rx_handle_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (samples == nullptr && sample_count > 0) {
        return ESP_ERR_INVALID_ARG;
    }
    size_t bytes_read = 0;
    esp_err_t err = i2s_channel_read(rx_handle_, samples, sample_count * sizeof(int16_t), &bytes_read, pdMS_TO_TICKS(100));
    samples_read = bytes_read / sizeof(int16_t);
    return err;
}

esp_err_t I2SAudio::stop() {
    esp_err_t result = ESP_OK;
    if (tx_handle_ != nullptr) {
        const esp_err_t err = i2s_channel_disable(tx_handle_);
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            result = err;
        }
    }
    if (rx_handle_ != nullptr) {
        const esp_err_t err = i2s_channel_disable(rx_handle_);
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            result = err;
        }
    }
    initialized_ = false;
    return result;
}

}  // namespace cardputer::hardware
