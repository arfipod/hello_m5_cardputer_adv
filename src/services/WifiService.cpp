#include "services/WifiService.hpp"

#include <algorithm>
#include <cstring>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

namespace cardputer::services {
namespace {
constexpr const char* TAG = "wifi_service";

#if APP_ENABLE_WIFI
bool s_netif_ready = false;
bool s_driver_ready = false;
bool s_station_started = false;
bool s_handlers_registered = false;
EventGroupHandle_t s_wifi_events = nullptr;
constexpr EventBits_t WIFI_CONNECTED_BIT = BIT0;

void onWifiEvent(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    (void)arg;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_wifi_events != nullptr) {
            xEventGroupClearBits(s_wifi_events, WIFI_CONNECTED_BIT);
        }
        const auto* event = static_cast<wifi_event_sta_disconnected_t*>(event_data);
        ESP_LOGW(TAG, "WiFi disconnected reason=%d", static_cast<int>(event->reason));
    }
}

void onIpEvent(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    (void)arg;
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        if (s_wifi_events != nullptr) {
            xEventGroupSetBits(s_wifi_events, WIFI_CONNECTED_BIT);
        }
        const auto* event = static_cast<ip_event_got_ip_t*>(event_data);
        ESP_LOGI(TAG, "WiFi got IPv4 " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

esp_err_t ensureNvsReady() {
    const esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE(TAG, "NVS needs maintenance before WiFi can start: %s", esp_err_to_name(err));
        return err;
    }
    return err;
}

bool isOpenAuth(wifi_auth_mode_t authmode) {
    return authmode == WIFI_AUTH_OPEN;
}

esp_err_t ensureEventHandlers() {
    if (s_wifi_events == nullptr) {
        s_wifi_events = xEventGroupCreate();
        if (s_wifi_events == nullptr) {
            return ESP_ERR_NO_MEM;
        }
    }
    if (s_handlers_registered) {
        return ESP_OK;
    }

    esp_err_t err = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, onWifiEvent, nullptr, nullptr);
    if (err != ESP_OK) {
        return err;
    }
    err = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, onIpEvent, nullptr, nullptr);
    if (err == ESP_OK) {
        s_handlers_registered = true;
    }
    return err;
}
#endif
}

esp_err_t WifiService::init() {
#if APP_ENABLE_WIFI
    if (s_driver_ready) {
        return ESP_OK;
    }

    esp_err_t err = ensureNvsReady();
    if (err != ESP_OK) {
        return err;
    }

    err = esp_netif_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }

    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }

    if (!s_netif_ready) {
        esp_netif_t* sta_netif = esp_netif_create_default_wifi_sta();
        if (sta_netif == nullptr) {
            return ESP_ERR_NO_MEM;
        }
        s_netif_ready = true;
    }

    err = ensureEventHandlers();
    if (err != ESP_OK) {
        return err;
    }

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&config);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    s_driver_ready = true;
    ESP_LOGI(TAG, "WiFi driver initialized; call startStation(), scan() or connectStation() next");
    return ESP_OK;
#else
    ESP_LOGW(TAG, "WiFi hook present but disabled; no bring-up sequence is linked");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t WifiService::startStation() {
#if APP_ENABLE_WIFI
    esp_err_t err = init();
    if (err != ESP_OK) {
        return err;
    }

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        return err;
    }

    err = esp_wifi_start();
    if (err != ESP_OK && err != ESP_ERR_WIFI_CONN) {
        return err;
    }
    s_station_started = true;
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t WifiService::connectStation(const char* ssid, const char* password) {
#if APP_ENABLE_WIFI
    if (ssid == nullptr || ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }
    if (std::strlen(ssid) > 32U) {
        return ESP_ERR_INVALID_ARG;
    }
    if (password != nullptr && std::strlen(password) >= 64U) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = startStation();
    if (err != ESP_OK) {
        return err;
    }

    wifi_config_t config {};
    std::strncpy(reinterpret_cast<char*>(config.sta.ssid), ssid, sizeof(config.sta.ssid));
    if (password != nullptr) {
        std::strncpy(reinterpret_cast<char*>(config.sta.password), password, sizeof(config.sta.password) - 1U);
    }

    err = esp_wifi_set_config(WIFI_IF_STA, &config);
    if (err != ESP_OK) {
        return err;
    }
    xEventGroupClearBits(s_wifi_events, WIFI_CONNECTED_BIT);
    return esp_wifi_connect();
#else
    (void)ssid;
    (void)password;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t WifiService::waitForConnection(uint32_t timeout_ms) {
#if APP_ENABLE_WIFI
    if (s_wifi_events == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    const EventBits_t bits = xEventGroupWaitBits(
        s_wifi_events,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(timeout_ms));
    return (bits & WIFI_CONNECTED_BIT) != 0 ? ESP_OK : ESP_ERR_TIMEOUT;
#else
    (void)timeout_ms;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

bool WifiService::isConnected() const {
#if APP_ENABLE_WIFI
    if (s_wifi_events == nullptr) {
        return false;
    }
    return (xEventGroupGetBits(s_wifi_events) & WIFI_CONNECTED_BIT) != 0;
#else
    return false;
#endif
}

esp_err_t WifiService::disconnect() {
#if APP_ENABLE_WIFI
    if (!s_driver_ready) {
        return ESP_ERR_INVALID_STATE;
    }
    const esp_err_t err = esp_wifi_disconnect();
    return err == ESP_ERR_WIFI_NOT_CONNECT ? ESP_OK : err;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t WifiService::scan(AccessPoint* results, uint16_t& count) {
#if APP_ENABLE_WIFI
    if (count > 0U && results == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = startStation();
    if (err != ESP_OK) {
        return err;
    }

    wifi_scan_config_t scan_config {};
    err = esp_wifi_scan_start(&scan_config, true);
    if (err != ESP_OK) {
        return err;
    }

    uint16_t found = 0;
    err = esp_wifi_scan_get_ap_num(&found);
    if (err != ESP_OK) {
        return err;
    }

    uint16_t copy_count = std::min<uint16_t>(count, std::min<uint16_t>(found, kMaxScanResults));
    wifi_ap_record_t raw[kMaxScanResults] {};
    err = esp_wifi_scan_get_ap_records(&copy_count, raw);
    if (err != ESP_OK) {
        return err;
    }

    for (uint16_t index = 0; index < copy_count; ++index) {
        AccessPoint& ap = results[index];
        std::strncpy(ap.ssid, reinterpret_cast<const char*>(raw[index].ssid), sizeof(ap.ssid) - 1U);
        ap.ssid[sizeof(ap.ssid) - 1U] = '\0';
        ap.rssi = raw[index].rssi;
        ap.channel = raw[index].primary;
        ap.encrypted = !isOpenAuth(raw[index].authmode);
    }
    count = copy_count;
    return ESP_OK;
#else
    (void)results;
    (void)count;
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t WifiService::stop() {
#if APP_ENABLE_WIFI
    if (!s_driver_ready) {
        return ESP_OK;
    }
    const esp_err_t err = esp_wifi_stop();
    if (err == ESP_OK || err == ESP_ERR_WIFI_NOT_INIT) {
        s_station_started = false;
        return ESP_OK;
    }
    if (err == ESP_ERR_WIFI_NOT_STARTED && s_station_started) {
        s_station_started = false;
        return ESP_OK;
    }
    return err;
#else
    return ESP_OK;
#endif
}

}  // namespace cardputer::services
