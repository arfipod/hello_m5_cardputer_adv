#pragma once

#include <cstdint>

namespace cardputer::app {

enum class AppEventType : uint8_t {
    None,
    Tick,
    PeripheralReady,
    PeripheralError,
};

struct AppEvent {
    AppEventType type = AppEventType::None;
    uint32_t value = 0;
};

}  // namespace cardputer::app
