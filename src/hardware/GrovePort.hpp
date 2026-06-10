#pragma once

#include "hardware/BoardPins.hpp"

namespace cardputer::hardware {

struct GrovePort {
    static constexpr auto g1 = pins::GROVE_G1;
    static constexpr auto g2 = pins::GROVE_G2;
};

}  // namespace cardputer::hardware
