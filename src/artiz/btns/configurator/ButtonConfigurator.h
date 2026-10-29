#pragma once

#include <Arduino.h>

#include "artiz/btns/core/ButtonItem.h"
#include "artiz/btns/configurator/Configuration.h"

class ButtonConfigurator
{
public:
    static void configureButtons(const Configuration& cfg)
    {
        config = cfg;

        for (size_t i = 0; i < config.itemCount; ++i)
        {
            pinMode(config.items[i].pin, config.mode);
        }
    }

    static const Configuration& getConfig()
    {
        return config;
    }

private:
    // Единственный общий объект конфигурации для всех компонентов библиотеки.
    inline static Configuration config{};
};