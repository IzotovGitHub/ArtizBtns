#pragma once

#include <Arduino.h>

#include "artiz/btns/core/ButtonItem.h"

class ButtonListener
{
public:
    template <size_t N>
    explicit ButtonListener(ButtonItem (&items)[N])
        : items(items), itemCount(N) {}

    void listenButtons();

private:
    ButtonItem *const items;
    const size_t itemCount;
};