#include "artiz/btns/listener/header/ButtonListener.h"

#include "artiz/btns/configurator/ButtonConfigurator.h"

namespace
{
ButtonState readButtonState(uint8_t pin)
{
    return digitalRead(pin) == LOW ? ButtonState::Pressed : ButtonState::Released;
}
}

void ButtonListener::listenButtons()
{
    const uint32_t now = millis();
    const Configuration &config = ButtonConfigurator::getConfig();

    for (size_t i = 0; i < config.itemCount; ++i)
    {
        ButtonItem &item = config.items[i];
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