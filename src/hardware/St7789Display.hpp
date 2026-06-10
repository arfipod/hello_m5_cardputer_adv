#pragma once

#include <cstddef>
#include <cstdint>

#include "driver/spi_master.h"
#include "esp_err.h"
#include "hal/spi_types.h"

namespace cardputer::hardware {

class St7789Display {
public:
    static constexpr uint16_t kWidth = 240;
    static constexpr uint16_t kHeight = 135;

    struct Config {
        spi_host_device_t host = SPI2_HOST;
        uint32_t pixel_clock_hz = 40000000;
    };

    St7789Display() = default;
    ~St7789Display();

    St7789Display(const St7789Display&) = delete;
    St7789Display& operator=(const St7789Display&) = delete;

    esp_err_t init();
    esp_err_t init(const Config& config);
    esp_err_t clear(uint16_t rgb565);
    esp_err_t drawPixels(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t* pixels);
    esp_err_t drawTextPlaceholder(uint16_t x, uint16_t y, const char* text, uint16_t rgb565);
    esp_err_t setBacklight(bool enabled);

private:
    Config config_{};
    spi_device_handle_t device_ = nullptr;
    bool bus_initialized_ = false;

    esp_err_t sendCommand(uint8_t command);
    esp_err_t sendData(const void* data, size_t length);
    esp_err_t setAddressWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
};

}  // namespace cardputer::hardware
