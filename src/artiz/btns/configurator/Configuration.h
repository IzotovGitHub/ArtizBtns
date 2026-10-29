#pragma once

#include <Arduino.h>

#include "artiz/btns/core/ButtonItem.h"

struct Configuration
{
    Configuration()
        : debounceDelayMs(50),
          mode(INPUT),
          items(nullptr),
          itemCount(0)
    {
    }

    template <size_t N>
    Configuration(
        uint32_t debounceDelayMs,
        uint8_t mode,
        ButtonItem (&items)[N])
        : debounceDelayMs(debounceDelayMs),
          mode(mode),
          items(items),
          itemCount(N)
    {
    }

    uint32_t debounceDelayMs;
    uint8_t mode;
    ButtonItem *items;
    size_t itemCount;
};
