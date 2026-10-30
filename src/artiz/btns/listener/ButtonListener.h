#pragma once

#include "artiz/btns/core/ButtonItem.h"
#include "artiz/btns/core/ButtonState.h"
#include "artiz/btns/core/ButtonTransition.h"

namespace
{
    static ButtonState readButtonState(uint8_t pin)
    {
        return digitalRead(pin) == LOW ? ButtonState::Pressed : ButtonState::Released;
    }
}

class ButtonListener
{
public:
    template <size_t N>
    static void updateState(ButtonItem (&items)[N])
    {
        const uint32_t now = millis();
        const Configuration &config = ButtonConfigurator::getConfig();

        for (size_t i = 0; i < N; ++i)
        {
            ButtonItem &item = items[i];
            const ButtonState currentRawState = readButtonState(item.pin);
            item.transition = ButtonTransition::None;

            if (currentRawState != item.rawState)
            {
                item.rawState = currentRawState;
                item.rawStateChangedAtMs = now;
            }

            if (item.rawState != item.state &&
                static_cast<uint32_t>(now - item.rawStateChangedAtMs) >= config.debounceDelayMs)
            {
                item.state = item.rawState;
                item.stateChangedAtMs = now;
                item.transition = item.state == ButtonState::Pressed
                                      ? ButtonTransition::Pressed
                                      : ButtonTransition::Released;
            }
        }
    }
};
