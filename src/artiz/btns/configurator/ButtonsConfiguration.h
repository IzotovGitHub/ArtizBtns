#pragma once

#include <Arduino.h>

struct ButtonsConfiguration
{
    ButtonsConfiguration()
        : debounceDelayMs(50),
          mode(INPUT),
          pins(nullptr),
          pinsCount(0)
    {
    }

    template <size_t N>
    ButtonsConfiguration(
        uint32_t debounceDelayMs,
        uint8_t mode,
        uint8_t (&pins)[N])
        : debounceDelayMs(debounceDelayMs),
          mode(mode),
          pins(pins),
          pinsCount(N)
    {
    }

    uint32_t debounceDelayMs;
    uint8_t mode;
    uint8_t *pins;
    size_t pinsCount;
};
