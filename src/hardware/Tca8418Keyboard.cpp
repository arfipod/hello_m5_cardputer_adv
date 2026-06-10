#include "hardware/Tca8418Keyboard.hpp"

#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {
namespace {
constexpr uint8_t REG_CFG = 0x01;
constexpr uint8_t REG_INT_STAT = 0x02;
constexpr uint8_t REG_KEY_LCK_EC = 0x03;
constexpr uint8_t REG_KEY_EVENT_A = 0x04;
constexpr uint8_t REG_KP_GPIO1 = 0x1D;
constexpr uint8_t REG_KP_GPIO2 = 0x1E;
constexpr uint8_t REG_KP_GPIO3 = 0x1F;

constexpr uint8_t CFG_KE_IEN = 0x01;
constexpr uint8_t CFG_OVR_FLOW_IEN = 0x02;
constexpr uint8_t INT_STAT_K_INT = 0x01;
constexpr uint8_t KEY_EVENT_RELEASE = 0x80;
constexpr uint8_t KEY_EVENT_CODE_MASK = 0x7F;
constexpr uint8_t KEY_EVENT_FIFO_MASK = 0x0F;

constexpr char kPlaceholderLayout[Tca8418Keyboard::kRows][Tca8418Keyboard::kColumns] = {
    {'1', '2', '3', '4', '5', '6', '7', '8'},
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k'},
    {'z', 'x', 'c', 'v', 'b', 'n', 'm', ','},
    {' ', '\n', '\b', '\t', 0, 0, 0, 0},
    {'9', '0', '-', '=', '[', ']', ';', '\''},
    {'o', 'p', 'l', '.', '/', 0, 0, 0},
};
}  // namespace

Tca8418Keyboard::Tca8418Keyboard(I2CBus& bus) : bus_(bus) {}

esp_err_t Tca8418Keyboard::init() {
    if (!bus_.isInitialized()) {
        return ESP_ERR_INVALID_STATE;
    }

    const gpio_config_t int_config = {
        .pin_bit_mask = 1ULL << pins::KEYBOARD_INT,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    esp_err_t err = gpio_config(&int_config);
    if (err != ESP_OK) {
        return err;
    }

    // Matrix configuration follows the TCA8418 register model: rows on KP_GPIO1
    // and the first eight columns on KP_GPIO2. Validate against the final board
    // schematic before treating this as a production keyboard driver.
    err = bus_.writeRegister(kAddress, REG_KP_GPIO1, 0x7F);
    if (err != ESP_OK) {
        return err;
    }
    err = bus_.writeRegister(kAddress, REG_KP_GPIO2, 0xFF);
    if (err != ESP_OK) {
        return err;
    }
    err = bus_.writeRegister(kAddress, REG_KP_GPIO3, 0x00);
    if (err != ESP_OK) {
        return err;
    }
    err = bus_.writeRegister(kAddress, REG_INT_STAT, 0xFF);
    if (err != ESP_OK) {
        return err;
    }
    err = bus_.writeRegister(kAddress, REG_CFG, CFG_KE_IEN | CFG_OVR_FLOW_IEN);
    if (err == ESP_OK) {
        initialized_ = true;
    }
    return err;
}

esp_err_t Tca8418Keyboard::readRawEvent(RawKeyEvent& event, bool& has_event) {
    has_event = false;
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t count = 0;
    esp_err_t err = bus_.readRegister(kAddress, REG_KEY_LCK_EC, &count, 1);
    if (err != ESP_OK) {
        return err;
    }
    if ((count & KEY_EVENT_FIFO_MASK) == 0) {
        return ESP_OK;
    }

    uint8_t raw_value = 0;
    err = bus_.readRegister(kAddress, REG_KEY_EVENT_A, &raw_value, 1);
    if (err != ESP_OK) {
        return err;
    }

    const uint8_t key_code = raw_value & KEY_EVENT_CODE_MASK;
    if (key_code == 0) {
        return ESP_OK;
    }

    const uint8_t zero_based = static_cast<uint8_t>(key_code - 1U);
    event.row = static_cast<uint8_t>(zero_based / kColumns);
    event.column = static_cast<uint8_t>(zero_based % kColumns);
    event.state = (raw_value & KEY_EVENT_RELEASE) != 0 ? KeyState::Released : KeyState::Pressed;
    has_event = event.row < kRows && event.column < kColumns;

    uint8_t int_status = INT_STAT_K_INT;
    (void)bus_.writeRegister(kAddress, REG_INT_STAT, int_status);
    return ESP_OK;
}

esp_err_t Tca8418Keyboard::readKeyEvent(KeyEvent& event, bool& has_event) {
    RawKeyEvent raw{};
    esp_err_t err = readRawEvent(raw, has_event);
    if (err != ESP_OK || !has_event) {
        return err;
    }
    event = mapRawEvent(raw);
    return ESP_OK;
}

Tca8418Keyboard::KeyEvent Tca8418Keyboard::mapRawEvent(const RawKeyEvent& raw) {
    KeyEvent event{};
    event.raw = raw;
    if (raw.row < kRows && raw.column < kColumns) {
        event.ascii = kPlaceholderLayout[raw.row][raw.column];
        event.logical_code = static_cast<uint16_t>((raw.row << 8U) | raw.column);
    }
    return event;
}

}  // namespace cardputer::hardware
