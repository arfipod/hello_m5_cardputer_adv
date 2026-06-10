#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "driver/i2c.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace cardputer::hardware {

class I2CBus {
public:
    struct Config {
        i2c_port_t port = I2C_NUM_0;
        uint32_t clock_hz = 400000;
        TickType_t timeout_ticks = pdMS_TO_TICKS(100);
    };

    I2CBus();
    ~I2CBus();

    I2CBus(const I2CBus&) = delete;
    I2CBus& operator=(const I2CBus&) = delete;

    esp_err_t init();
    esp_err_t init(const Config& config);
    bool isInitialized() const;

    esp_err_t write(uint8_t address, const uint8_t* data, size_t length);
    esp_err_t read(uint8_t address, uint8_t* data, size_t length);
    esp_err_t writeRegister(uint8_t address, uint8_t reg, uint8_t value);
    esp_err_t writeRegister(uint8_t address, uint8_t reg, const uint8_t* data, size_t length);
    esp_err_t readRegister(uint8_t address, uint8_t reg, uint8_t* data, size_t length);

    esp_err_t scan(std::array<uint8_t, 128>& addresses, size_t& count);

private:
    esp_err_t take();
    void give();

    Config config_{};
    SemaphoreHandle_t mutex_ = nullptr;
    bool initialized_ = false;
};

}  // namespace cardputer::hardware
