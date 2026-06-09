#pragma once

#include <cstdint>

namespace cardputer::app {

enum class AppRunState : uint8_t {
    Booting,
    Running,
    Error,
};

struct AppState {
    AppRunState run_state = AppRunState::Booting;
    uint32_t uptime_seconds = 0;
};

}  // namespace cardputer::app
