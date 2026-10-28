#pragma once

#include  <Arduino.h>

enum class ButtonEvent : uint8_t
{
    None,
    Click,
    DoubleClick,
    LongPress
};