# ArtizBtns

ArtizBtns is an Arduino library for configuring button pins and polling their
debounced physical state, including on ESP32 projects built with PlatformIO.

## Features

- Stores the GPIO pin and current sampling data for each button in `ButtonItem`.
- Configures button pins with a selected Arduino pin mode.
- Debounces button state changes using the configured delay.
- Exposes the stable state (`Pressed` or `Released`) and the transition detected
  during the latest polling call (`Pressed`, `Released`, or `None`).

Click, double-click, and long-press event detection is not implemented yet.
`ButtonEvent` currently only defines event names.

## Basic usage

```cpp
#include <artiz/btns/configurator/ButtonConfigurator.h>
#include <artiz/btns/configurator/ButtonsConfiguration.h>
#include <artiz/btns/core/ButtonItem.h>
#include <artiz/btns/listener/ButtonListener.h>

const uint8_t buttonPins[] = {18, 19};
ButtonItem buttons[] = {ButtonItem(18), ButtonItem(19)};

void setup()
{
    ButtonConfigurator::configureButtons(
        ButtonsConfiguration(50, INPUT_PULLUP, buttonPins));
}

void loop()
{
    ButtonListener::updateState(buttons);

    for (ButtonItem &button : buttons)
    {
        if (button.transition == ButtonTransition::Pressed)
        {
            // The button became pressed after debounce.
        }
    }
}
```

Keep the pin array alive as long as the shared configuration uses it. The
listener treats `LOW` as pressed and `HIGH` as released, matching buttons wired
between a GPIO and GND with `INPUT_PULLUP`. `ButtonItem::transition` reports a
confirmed transition for one polling call and resets to `None` on the next call
if there is no new transition. Poll `updateState()` regularly from `loop()`.

## Tests

Run the host-side unit tests without a connected board:

```sh
pio test -e native
```

The native tests use a small fake Arduino API to control GPIO levels and
`millis()`. They cover pin configuration, initial state, press/release
debouncing, bounce handling, independent buttons, one-poll transitions, and
`millis()` rollover.
