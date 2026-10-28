#pragma once

#include <Arduino.h>

enum class ButtonTransition : uint8_t
{
    None,
    Pressed,
    Released
};