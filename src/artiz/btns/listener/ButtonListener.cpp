#include "artiz/btns/listener/header/ButtonListener.h"

namespace
{
constexpr uint32_t debounceDelayMs = 50;

ButtonState readButtonState(uint8_t pin)
{
    return digitalRead(pin) == LOW ? ButtonState::Pressed : ButtonState::Released;
}
}

void ButtonListener::listenButtons()
{
    const uint32_t now = millis();

    for (size_t i = 0; i < itemCount; ++i)
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
            static_cast<uint32_t>(now - item.rawStateChangedAtMs) >= debounceDelayMs)
        {
            item.state = item.rawState;
            item.stateChangedAtMs = now;
            item.transition = item.state == ButtonState::Pressed
                ? ButtonTransition::Pressed
                : ButtonTransition::Released;
        }
    }
}