#pragma once

#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

struct ExtBus {
    static constexpr auto reset = pins::EXT_RESET_G3;
    static constexpr auto interrupt = pins::EXT_INT_G4;
    static constexpr auto chip_select = pins::EXT_CS_G5;
    static constexpr auto busy = pins::EXT_BUSY_G6;
    static constexpr auto i2c_sda = pins::EXT_I2C_SDA_G8;
    static constexpr auto i2c_scl = pins::EXT_I2C_SCL_G9;
    static constexpr auto spi_sck = pins::EXT_SPI_SCK_G40;
    static constexpr auto spi_mosi = pins::EXT_SPI_MOSI_G14;
    static constexpr auto spi_miso = pins::EXT_SPI_MISO_G39;
    static constexpr auto uart_rx = pins::EXT_UART_RX_G13;
    static constexpr auto uart_tx = pins::EXT_UART_TX_G15;
};

}  // namespace cardputer::hardware
