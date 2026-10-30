#pragma once

#include <Arduino.h>

#include "artiz/btns/core/ButtonItem.h"
#include "artiz/btns/configurator/ButtonsConfiguration.h"

class ButtonConfigurator
{
public:
    static void configureButtons(const ButtonsConfiguration& cfg)
    {
        config = cfg;

        for (size_t i = 0; i < config.pinsCount; ++i)
        {
            pinMode(config.pins[i], config.mode);
        }
    }

    static const ButtonsConfiguration& getConfig()
    {
        return config;
    }

private:
    // Единственный общий объект конфигурации для всех компонентов библиотеки.
    inline static ButtonsConfiguration config{};
};