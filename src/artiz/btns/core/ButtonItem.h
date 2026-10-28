#pragma once

#include <Arduino.h>

#include "artiz/btns/core/ButtonState.h"
#include "artiz/btns/core/ButtonTransition.h"

struct ButtonItem
{
    ButtonItem(uint8_t pinValue)
        : pin(pinValue),
          state(ButtonState::Released),
          rawState(ButtonState::Released),
          transition(ButtonTransition::None),
          rawStateChangedAtMs(0),
          stateChangedAtMs(0) {}

    const uint8_t pin;                    // Номер GPIO, к которому подключена кнопка.
    ButtonState state;                    // Подтверждённое состояние после debounce.
    ButtonState rawState;                 // Последнее состояние, считанное с GPIO.
    ButtonTransition transition;          // Переход состояния, обнаруженный в текущем проходе.
    uint32_t rawStateChangedAtMs;         // Время последнего изменения rawState (millis()).
    uint32_t stateChangedAtMs;            // Время последнего подтверждённого изменения state.
};