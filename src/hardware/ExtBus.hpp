#pragma once

#include "driver/gpio.h"
#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

struct ExtBus {
    static constexpr gpio_num_t reset = pins::EXT_RESET_G3;
    static constexpr gpio_num_t interrupt = pins::EXT_INT_G4;
    static constexpr gpio_num_t chip_select = pins::EXT_CS_G5;
    static constexpr gpio_num_t busy = pins::EXT_BUSY_G6;
    static constexpr gpio_num_t i2c_sda = pins::EXT_I2C_SDA_G8;
    static constexpr gpio_num_t i2c_scl = pins::EXT_I2C_SCL_G9;
    static constexpr gpio_num_t spi_sck = pins::EXT_SPI_SCK_G40;
    static constexpr gpio_num_t spi_mosi = pins::EXT_SPI_MOSI_G14;
    static constexpr gpio_num_t spi_miso = pins::EXT_SPI_MISO_G39;
    static constexpr gpio_num_t uart_rx = pins::EXT_UART_RX_G13;
    static constexpr gpio_num_t uart_tx = pins::EXT_UART_TX_G15;
};

}  // namespace cardputer::hardware
