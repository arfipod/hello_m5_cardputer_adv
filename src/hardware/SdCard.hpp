#pragma once

#include "esp_err.h"
#include "hal/spi_types.h"
#include "sdmmc_cmd.h"

namespace cardputer::hardware {

class SdCard {
public:
    struct Config {
        const char* mount_point = "/sdcard";
        spi_host_device_t host = SPI3_HOST;
        bool format_if_mount_failed = false;
    };

    SdCard() = default;
    ~SdCard();

    SdCard(const SdCard&) = delete;
    SdCard& operator=(const SdCard&) = delete;

    esp_err_t mount();
    esp_err_t mount(const Config& config);
    esp_err_t unmount();
    bool isMounted() const;
    const sdmmc_card_t* card() const;

private:
    Config config_{};
    sdmmc_card_t* card_ = nullptr;
    bool bus_initialized_ = false;
};

}  // namespace cardputer::hardware
