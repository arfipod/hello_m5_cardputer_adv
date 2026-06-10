#pragma once

#include "driver/gpio.h"
#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

struct GrovePort {
    static constexpr gpio_num_t g1 = pins::GROVE_G1;
    static constexpr gpio_num_t g2 = pins::GROVE_G2;
};

}  // namespace cardputer::hardware
