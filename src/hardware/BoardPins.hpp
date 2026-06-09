#pragma once

#include "driver/gpio.h"

namespace cardputer::hardware::pins {

static constexpr gpio_num_t I2C_SDA = GPIO_NUM_8;
static constexpr gpio_num_t I2C_SCL = GPIO_NUM_9;

static constexpr gpio_num_t KEYBOARD_INT = GPIO_NUM_11;
static constexpr gpio_num_t BATTERY_ADC = GPIO_NUM_10;

static constexpr gpio_num_t SD_CS = GPIO_NUM_12;
static constexpr gpio_num_t SD_MOSI = GPIO_NUM_14;
static constexpr gpio_num_t SD_CLK = GPIO_NUM_40;
static constexpr gpio_num_t SD_MISO = GPIO_NUM_39;

static constexpr gpio_num_t LCD_BL = GPIO_NUM_38;
static constexpr gpio_num_t LCD_RST = GPIO_NUM_33;
static constexpr gpio_num_t LCD_DC = GPIO_NUM_34;
static constexpr gpio_num_t LCD_MOSI = GPIO_NUM_35;
static constexpr gpio_num_t LCD_SCK = GPIO_NUM_36;
static constexpr gpio_num_t LCD_CS = GPIO_NUM_37;

static constexpr gpio_num_t I2S_BCLK = GPIO_NUM_41;
static constexpr gpio_num_t I2S_DOUT_TO_CODEC_DSDIN = GPIO_NUM_42;
static constexpr gpio_num_t I2S_LRCLK = GPIO_NUM_43;
static constexpr gpio_num_t I2S_DIN_FROM_CODEC_ASDOUT = GPIO_NUM_46;

static constexpr gpio_num_t IR_TX = GPIO_NUM_44;

static constexpr gpio_num_t GROVE_G1 = GPIO_NUM_1;
static constexpr gpio_num_t GROVE_G2 = GPIO_NUM_2;

static constexpr gpio_num_t EXT_RESET_G3 = GPIO_NUM_3;
static constexpr gpio_num_t EXT_INT_G4 = GPIO_NUM_4;
static constexpr gpio_num_t EXT_CS_G5 = GPIO_NUM_5;
static constexpr gpio_num_t EXT_BUSY_G6 = GPIO_NUM_6;
static constexpr gpio_num_t EXT_I2C_SDA_G8 = GPIO_NUM_8;
static constexpr gpio_num_t EXT_I2C_SCL_G9 = GPIO_NUM_9;
static constexpr gpio_num_t EXT_SPI_SCK_G40 = GPIO_NUM_40;
static constexpr gpio_num_t EXT_SPI_MOSI_G14 = GPIO_NUM_14;
static constexpr gpio_num_t EXT_SPI_MISO_G39 = GPIO_NUM_39;
static constexpr gpio_num_t EXT_UART_RX_G13 = GPIO_NUM_13;
static constexpr gpio_num_t EXT_UART_TX_G15 = GPIO_NUM_15;

static constexpr gpio_num_t BOOT = GPIO_NUM_0;

}  // namespace cardputer::hardware::pins
