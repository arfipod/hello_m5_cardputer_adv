#include "hardware/St7789Display.hpp"

#include <vector>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {
namespace {
constexpr uint8_t CMD_SWRESET = 0x01;
constexpr uint8_t CMD_SLPOUT = 0x11;
constexpr uint8_t CMD_INVON = 0x21;
constexpr uint8_t CMD_CASET = 0x2A;
constexpr uint8_t CMD_RASET = 0x2B;
constexpr uint8_t CMD_RAMWR = 0x2C;
constexpr uint8_t CMD_MADCTL = 0x36;
constexpr uint8_t CMD_COLMOD = 0x3A;
constexpr uint8_t CMD_DISPON = 0x29;
}  // namespace

St7789Display::~St7789Display() {
    if (device_ != nullptr) {
        spi_bus_remove_device(device_);
    }
    if (bus_initialized_) {
        spi_bus_free(config_.host);
    }
}

esp_err_t St7789Display::init() {
    return init(Config{});
}

esp_err_t St7789Display::init(const Config& config) {
    if (device_ != nullptr) {
        return ESP_OK;
    }
    config_ = config;

    gpio_config_t output_config{};
    output_config.pin_bit_mask = (1ULL << pins::LCD_BL) | (1ULL << pins::LCD_DC) | (1ULL << pins::LCD_RST);
    output_config.mode = GPIO_MODE_OUTPUT;
    output_config.pull_up_en = GPIO_PULLUP_DISABLE;
    output_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    output_config.intr_type = GPIO_INTR_DISABLE;
    esp_err_t err = gpio_config(&output_config);
    if (err != ESP_OK) {
        return err;
    }
    gpio_set_level(pins::LCD_BL, 0);
    gpio_set_level(pins::LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(pins::LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    spi_bus_config_t bus_config{};
    bus_config.mosi_io_num = pins::LCD_MOSI;
    bus_config.miso_io_num = -1;
    bus_config.sclk_io_num = pins::LCD_SCK;
    bus_config.quadwp_io_num = -1;
    bus_config.quadhd_io_num = -1;
    bus_config.data4_io_num = -1;
    bus_config.data5_io_num = -1;
    bus_config.data6_io_num = -1;
    bus_config.data7_io_num = -1;
    bus_config.data_io_default_level = false;
    bus_config.max_transfer_sz = kWidth * 40 * sizeof(uint16_t);
    bus_config.flags = SPICOMMON_BUSFLAG_MASTER;
    bus_config.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    err = spi_bus_initialize(config_.host, &bus_config, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }
    bus_initialized_ = err == ESP_OK;

    spi_device_interface_config_t device_config{};
    device_config.clock_speed_hz = static_cast<int>(config_.pixel_clock_hz);
    device_config.mode = 0;
    device_config.spics_io_num = pins::LCD_CS;
    device_config.queue_size = 7;
    device_config.flags = SPI_DEVICE_HALFDUPLEX;
    err = spi_bus_add_device(config_.host, &device_config, &device_);
    if (err != ESP_OK) {
        return err;
    }

    err = sendCommand(CMD_SWRESET);
    if (err != ESP_OK) {
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(120));

    err = sendCommand(CMD_SLPOUT);
    if (err != ESP_OK) {
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(120));

    const uint8_t color_mode = 0x55;
    err = sendCommand(CMD_COLMOD);
    if (err != ESP_OK) {
        return err;
    }
    err = sendData(&color_mode, sizeof(color_mode));
    if (err != ESP_OK) {
        return err;
    }

    const uint8_t madctl = 0x00;
    err = sendCommand(CMD_MADCTL);
    if (err != ESP_OK) {
        return err;
    }
    err = sendData(&madctl, sizeof(madctl));
    if (err != ESP_OK) {
        return err;
    }

    err = sendCommand(CMD_INVON);
    if (err != ESP_OK) {
        return err;
    }
    err = sendCommand(CMD_DISPON);
    if (err != ESP_OK) {
        return err;
    }
    return setBacklight(true);
}

esp_err_t St7789Display::clear(uint16_t rgb565) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    std::vector<uint16_t> line(kWidth, rgb565);
    for (uint16_t y = 0; y < kHeight; ++y) {
        esp_err_t err = drawPixels(0, y, kWidth, 1, line.data());
        if (err != ESP_OK) {
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t St7789Display::drawPixels(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t* pixels) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (pixels == nullptr || width == 0 || height == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = setAddressWindow(x, y, width, height);
    if (err != ESP_OK) {
        return err;
    }
    err = sendCommand(CMD_RAMWR);
    if (err != ESP_OK) {
        return err;
    }
    return sendData(pixels, width * height * sizeof(uint16_t));
}

esp_err_t St7789Display::drawTextPlaceholder(uint16_t x, uint16_t y, const char* text, uint16_t rgb565) {
    (void)x;
    (void)y;
    (void)text;
    (void)rgb565;
    return device_ == nullptr ? ESP_ERR_INVALID_STATE : ESP_ERR_NOT_SUPPORTED;
}

esp_err_t St7789Display::setBacklight(bool enabled) {
    return gpio_set_level(pins::LCD_BL, enabled ? 1 : 0);
}

esp_err_t St7789Display::sendCommand(uint8_t command) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    spi_transaction_t transaction{};
    transaction.length = 8;
    transaction.flags = SPI_TRANS_USE_TXDATA;
    transaction.tx_data[0] = command;
    gpio_set_level(pins::LCD_DC, 0);
    return spi_device_transmit(device_, &transaction);
}

esp_err_t St7789Display::sendData(const void* data, size_t length) {
    if (device_ == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    if (data == nullptr || length == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    spi_transaction_t transaction{};
    transaction.length = length * 8;
    transaction.tx_buffer = data;
    gpio_set_level(pins::LCD_DC, 1);
    return spi_device_transmit(device_, &transaction);
}

esp_err_t St7789Display::setAddressWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    const uint16_t x_end = static_cast<uint16_t>(x + width - 1U);
    const uint16_t y_end = static_cast<uint16_t>(y + height - 1U);
    const uint8_t column_data[] = {
        static_cast<uint8_t>(x >> 8U), static_cast<uint8_t>(x & 0xFFU),
        static_cast<uint8_t>(x_end >> 8U), static_cast<uint8_t>(x_end & 0xFFU),
    };
    const uint8_t row_data[] = {
        static_cast<uint8_t>(y >> 8U), static_cast<uint8_t>(y & 0xFFU),
        static_cast<uint8_t>(y_end >> 8U), static_cast<uint8_t>(y_end & 0xFFU),
    };

    esp_err_t err = sendCommand(CMD_CASET);
    if (err != ESP_OK) {
        return err;
    }
    err = sendData(column_data, sizeof(column_data));
    if (err != ESP_OK) {
        return err;
    }
    err = sendCommand(CMD_RASET);
    if (err != ESP_OK) {
        return err;
    }
    return sendData(row_data, sizeof(row_data));
}

}  // namespace cardputer::hardware
