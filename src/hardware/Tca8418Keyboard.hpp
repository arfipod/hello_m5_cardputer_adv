#pragma once

#include <cstdint>

#include "driver/gpio.h"
#include "esp_err.h"
#include "hardware/I2CBus.hpp"

namespace cardputer::hardware {

class Tca8418Keyboard {
public:
    static constexpr uint8_t kAddress = 0x34;
    static constexpr uint8_t kRows = 7;
    static constexpr uint8_t kColumns = 8;

    enum class KeyState : uint8_t {
        Pressed,
        Released,
    };

    struct RawKeyEvent {
        uint8_t row = 0;
        uint8_t column = 0;
        KeyState state = KeyState::Released;
    };

    struct KeyEvent {
        RawKeyEvent raw{};
        char ascii = '\0';
        uint16_t logical_code = 0;
    };

    explicit Tca8418Keyboard(I2CBus& bus);

    esp_err_t init();
    esp_err_t readRawEvent(RawKeyEvent& event, bool& has_event);
    esp_err_t readKeyEvent(KeyEvent& event, bool& has_event);

    static KeyEvent mapRawEvent(const RawKeyEvent& raw);

private:
    I2CBus& bus_;
    bool initialized_ = false;
};

}  // namespace cardputer::hardware
