#include "hardware/SdCard.hpp"

#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_vfs_fat.h"
#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

SdCard::~SdCard() {
    unmount();
}

esp_err_t SdCard::mount() {
    return mount(Config{});
}

esp_err_t SdCard::mount(const Config& config) {
    if (card_ != nullptr) {
        return ESP_OK;
    }
    config_ = config;

    const spi_bus_config_t bus_config = {
        .mosi_io_num = pins::SD_MOSI,
        .miso_io_num = pins::SD_MISO,
        .sclk_io_num = pins::SD_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .data_io_default_level = false,
        .max_transfer_sz = 4000,
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
        .intr_flags = 0,
    };
    esp_err_t err = spi_bus_initialize(config_.host, &bus_config, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }
    bus_initialized_ = err == ESP_OK;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = config_.host;

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = pins::SD_CS;
    slot_config.host_id = config_.host;

    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = config_.format_if_mount_failed,
        .max_files = 4,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = false,
        .use_one_fat = false,
    };

    err = esp_vfs_fat_sdspi_mount(config_.mount_point, &host, &slot_config, &mount_config, &card_);
    if (err != ESP_OK && bus_initialized_) {
        spi_bus_free(config_.host);
        bus_initialized_ = false;
    }
    return err;
}

esp_err_t SdCard::unmount() {
    esp_err_t result = ESP_OK;
    if (card_ != nullptr) {
        result = esp_vfs_fat_sdcard_unmount(config_.mount_point, card_);
        card_ = nullptr;
    }
    if (bus_initialized_) {
        const esp_err_t err = spi_bus_free(config_.host);
        if (result == ESP_OK) {
            result = err;
        }
        bus_initialized_ = false;
    }
    return result;
}

bool SdCard::isMounted() const {
    return card_ != nullptr;
}

const sdmmc_card_t* SdCard::card() const {
    return card_;
}

}  // namespace cardputer::hardware
