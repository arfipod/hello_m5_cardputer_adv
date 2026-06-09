#include "app/App.hpp"

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"

extern "C" void app_main(void) {
    static constexpr const char* TAG = "main";

    esp_log_level_set("*", ESP_LOG_INFO);

    esp_chip_info_t chip_info {};
    esp_chip_info(&chip_info);

    uint32_t flash_size = 0;
    esp_flash_get_size(nullptr, &flash_size);

    ESP_LOGI(TAG, "Cardputer-Adv template starting");
    ESP_LOGI(TAG, "CPU cores: %d", chip_info.cores);
    ESP_LOGI(TAG, "Chip features: 0x%lx", static_cast<unsigned long>(chip_info.features));
    ESP_LOGI(TAG, "Flash size: %lu MB", static_cast<unsigned long>(flash_size / (1024 * 1024)));

    cardputer::app::App app;
    app.run();
}
