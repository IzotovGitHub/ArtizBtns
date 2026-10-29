# ArtizBtns

ArtizBtns is a small Arduino library for configuring button pins and polling
their debounced physical state. It is intended for use in Arduino projects,
including ESP32 projects built with PlatformIO.

## Current features

- Stores the GPIO pin and current sampling data for each button in `ButtonItem`.
- Configures an array of button pins with a selected Arduino pin mode.
- Polls all configured buttons with `ButtonListener::listenButtons()`.
- Debounces state changes with a fixed 50 ms interval.
- Exposes the stable state (`Pressed` or `Released`) and the transition detected
  during the latest polling call (`Pressed`, `Released`, or `None`).

Click, double-click, and long-press event detection is not implemented yet.
`ButtonEvent` currently only defines event names.

## Basic usage

```cpp
#include <artiz/btns/core/ButtonItem.h>
#include <artiz/btns/configurator/ButtonConfigurator.h>
#include <artiz/btns/listener/header/ButtonListener.h>

ButtonItem buttons[] = {
    ButtonItem(18),
    ButtonItem(19)
};

Configuration buttonConfig(50, INPUT_PULLUP, buttons);
ButtonListener buttonListener;

void setup()
{
    ButtonConfigurator::configureButtons(buttonConfig);
}

void loop()
{
    buttonListener.listenButtons();

    for (ButtonItem &button : buttons)
    {
        if (button.transition == ButtonTransition::Pressed)
        {
            // The button became pressed after debounce.
        }
        else if (button.transition == ButtonTransition::Released)
        {
            // The button became released after debounce.
        }
    }
}
```

The configuration stores a pointer to the `ButtonItem` array, and the listener
uses the shared configuration saved by `ButtonConfigurator`. Keep both the
configuration and its button array alive for as long as the library uses them.

## Input behavior

The listener currently treats `LOW` as pressed and `HIGH` as released. This
matches buttons wired between a GPIO and GND when configured with
`INPUT_PULLUP`. Although `configureButtons()` accepts a pin mode argument,
other active-level conventions are not currently configurable in the listener.

`ButtonItem::state` holds the debounced state. `ButtonItem::transition` is set
to the confirmed transition on the polling call where it occurs, and is reset
to `None` on the next call if there is no new transition. Call
`listenButtons()` regularly from `loop()` and inspect the transition after each
call.
