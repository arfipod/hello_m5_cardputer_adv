#include "hardware/IrTransmitter.hpp"

#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

IrTransmitter::~IrTransmitter() {
    if (channel_ != nullptr) {
        rmt_disable(channel_);
    }
    if (copy_encoder_ != nullptr) {
        rmt_del_encoder(copy_encoder_);
    }
    if (channel_ != nullptr) {
        rmt_del_channel(channel_);
    }
}

esp_err_t IrTransmitter::init() {
    return init(Config{});
}

esp_err_t IrTransmitter::init(const Config& config) {
    if (initialized_) {
        return ESP_OK;
    }

    const rmt_tx_channel_config_t tx_config = {
        .gpio_num = pins::IR_TX,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = config.resolution_hz,
        .mem_block_symbols = 64,
        .trans_queue_depth = 4,
        .intr_priority = 0,
        .flags = {
            .invert_out = false,
            .with_dma = false,
            .io_loop_back = false,
            .io_od_mode = false,
            .allow_pd = false,
        },
    };
    esp_err_t err = rmt_new_tx_channel(&tx_config, &channel_);
    if (err != ESP_OK) {
        return err;
    }

    const rmt_carrier_config_t carrier_config = {
        .frequency_hz = config.carrier_hz,
        .duty_cycle = 0.33f,
        .flags = {
            .polarity_active_low = false,
            .always_on = false,
        },
    };
    err = rmt_apply_carrier(channel_, &carrier_config);
    if (err != ESP_OK) {
        return err;
    }

    const rmt_copy_encoder_config_t encoder_config = {};
    err = rmt_new_copy_encoder(&encoder_config, &copy_encoder_);
    if (err != ESP_OK) {
        return err;
    }

    err = rmt_enable(channel_);
    if (err == ESP_OK) {
        initialized_ = true;
    }
    return err;
}

esp_err_t IrTransmitter::transmitRaw(const rmt_symbol_word_t* symbols, size_t symbol_count) {
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }
    if (symbols == nullptr || symbol_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    const rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
        .flags = {
            .eot_level = 0,
            .queue_nonblocking = false,
        },
    };
    return rmt_transmit(channel_, copy_encoder_, symbols, symbol_count * sizeof(rmt_symbol_word_t), &transmit_config);
}

}  // namespace cardputer::hardware
