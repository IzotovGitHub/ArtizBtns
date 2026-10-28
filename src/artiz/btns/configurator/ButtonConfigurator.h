#pragma once

#include <Arduino.h>

#include "artiz/btns/core/ButtonItem.h"

class ButtonConfigurator
{
public:
    template <size_t N>
    static void configureButtons(ButtonItem (&items)[N], uint8_t mode)
    {
        for (size_t i = 0; i < N; ++i)
        {
            pinMode(items[i].pin, mode);
        }
    }
};
