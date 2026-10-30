#include <unity.h>

#include <artiz/btns/configurator/ButtonConfigurator.h>
#include <artiz/btns/configurator/ButtonsConfiguration.h>
#include <artiz/btns/core/ButtonItem.h>
#include <artiz/btns/core/ButtonState.h>
#include <artiz/btns/core/ButtonTransition.h>
#include <artiz/btns/listener/ButtonListener.h>

namespace
{
    uint32_t currentTimeMs = 0;
    int pinLevels[256];
    int configuredModes[256];
    uint8_t configuredPins[256];
    size_t configuredPinCount = 0;

    void setPinLevel(uint8_t pin, int level)
    {
        pinLevels[pin] = level;
    }

    void resetFakes()
    {
        currentTimeMs = 0;
        configuredPinCount = 0;

        for (size_t i = 0; i < 256; ++i)
        {
            pinLevels[i] = HIGH;
            configuredModes[i] = -1;
            configuredPins[i] = 0;
        }
    }
}

unsigned long millis()
{
    return currentTimeMs;
}

int digitalRead(uint8_t pin)
{
    return pinLevels[pin];
}

void pinMode(uint8_t pin, uint8_t mode)
{
    configuredModes[pin] = mode;
    configuredPins[configuredPinCount++] = pin;
}

void setUp()
{
    resetFakes();
}

void tearDown()
{
}

// Проверяет начальные значения GPIO, состояний и перехода новой кнопки.
void test_button_item_starts_released_with_no_transition()
{
    const ButtonItem button(25);

    TEST_ASSERT_EQUAL_UINT8(25, button.pin);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(button.state));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(button.rawState));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonTransition::None), static_cast<int>(button.transition));
}

// Проверяет, что конфигуратор сохраняет параметры и задаёт режим каждому пину.
void test_configurator_sets_mode_for_each_pin_and_stores_configuration()
{
    const uint8_t pins[] = {25, 26, 27};
    const ButtonsConfiguration config(35, INPUT_PULLUP, pins);

    ButtonConfigurator::configureButtons(config);
    const ButtonsConfiguration &stored = ButtonConfigurator::getConfig();

    TEST_ASSERT_EQUAL_UINT32(35, stored.debounceDelayMs);
    TEST_ASSERT_EQUAL_UINT8(INPUT_PULLUP, stored.mode);
    TEST_ASSERT_EQUAL_UINT32(3, stored.pinsCount);
    TEST_ASSERT_EQUAL_UINT8(25, stored.pins[0]);
    TEST_ASSERT_EQUAL_UINT8(26, stored.pins[1]);
    TEST_ASSERT_EQUAL_UINT8(27, stored.pins[2]);
    TEST_ASSERT_EQUAL_UINT32(3, configuredPinCount);
    TEST_ASSERT_EQUAL_UINT8(25, configuredPins[0]);
    TEST_ASSERT_EQUAL_UINT8(26, configuredPins[1]);
    TEST_ASSERT_EQUAL_UINT8(27, configuredPins[2]);
    TEST_ASSERT_EQUAL_INT(INPUT_PULLUP, configuredModes[25]);
    TEST_ASSERT_EQUAL_INT(INPUT_PULLUP, configuredModes[26]);
    TEST_ASSERT_EQUAL_INT(INPUT_PULLUP, configuredModes[27]);
}

// Проверяет, что нажатие подтверждается только после полной задержки debounce.
void test_press_is_not_confirmed_before_debounce_delay()
{
    const uint8_t pins[] = {25};
    const ButtonsConfiguration config(50, INPUT_PULLUP, pins);
    ButtonConfigurator::configureButtons(config);
    ButtonItem buttons[] = {ButtonItem(25)};

    currentTimeMs = 100;
    setPinLevel(25, LOW);
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(buttons[0].state));

    currentTimeMs = 149;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(buttons[0].state));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonTransition::None), static_cast<int>(buttons[0].transition));

    currentTimeMs = 150;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Pressed), static_cast<int>(buttons[0].state));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonTransition::Pressed), static_cast<int>(buttons[0].transition));
}

// Проверяет, что подтверждённый переход выдаётся один раз, а затем сбрасывается.
void test_transition_is_reported_for_only_one_poll()
{
    const uint8_t pins[] = {25};
    ButtonConfigurator::configureButtons(ButtonsConfiguration(10, INPUT_PULLUP, pins));
    ButtonItem buttons[] = {ButtonItem(25)};

    setPinLevel(25, LOW);
    ButtonListener::updateState(buttons);
    currentTimeMs = 10;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonTransition::Pressed), static_cast<int>(buttons[0].transition));

    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonTransition::None), static_cast<int>(buttons[0].transition));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Pressed), static_cast<int>(buttons[0].state));
}

// Проверяет, что смена уровня при дребезге заново запускает таймер debounce.
void test_bounce_restarts_debounce_timer()
{
    const uint8_t pins[] = {25};
    ButtonConfigurator::configureButtons(ButtonsConfiguration(50, INPUT_PULLUP, pins));
    ButtonItem buttons[] = {ButtonItem(25)};

    setPinLevel(25, LOW);
    ButtonListener::updateState(buttons);

    currentTimeMs = 30;
    setPinLevel(25, HIGH);
    ButtonListener::updateState(buttons);

    currentTimeMs = 79;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(buttons[0].state));

    currentTimeMs = 80;
    setPinLevel(25, LOW);
    ButtonListener::updateState(buttons);
    currentTimeMs = 129;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(buttons[0].state));

    currentTimeMs = 130;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Pressed), static_cast<int>(buttons[0].state));
}

// Проверяет, что отпускание тоже подтверждается лишь после задержки debounce.
void test_release_is_debounced()
{
    const uint8_t pins[] = {25};
    ButtonConfigurator::configureButtons(ButtonsConfiguration(20, INPUT_PULLUP, pins));
    ButtonItem buttons[] = {ButtonItem(25)};

    setPinLevel(25, LOW);
    ButtonListener::updateState(buttons);
    currentTimeMs = 20;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Pressed), static_cast<int>(buttons[0].state));

    currentTimeMs = 21;
    setPinLevel(25, HIGH);
    ButtonListener::updateState(buttons);
    currentTimeMs = 40;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Pressed), static_cast<int>(buttons[0].state));

    currentTimeMs = 41;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(buttons[0].state));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonTransition::Released), static_cast<int>(buttons[0].transition));
}

// Проверяет, что debounce одной кнопки не влияет на таймер другой кнопки.
void test_buttons_are_debounced_independently()
{
    const uint8_t pins[] = {25, 26};
    ButtonConfigurator::configureButtons(ButtonsConfiguration(10, INPUT_PULLUP, pins));
    ButtonItem buttons[] = {ButtonItem(25), ButtonItem(26)};

    setPinLevel(25, LOW);
    ButtonListener::updateState(buttons);
    currentTimeMs = 5;
    setPinLevel(26, LOW);
    ButtonListener::updateState(buttons);

    currentTimeMs = 10;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Pressed), static_cast<int>(buttons[0].state));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(buttons[1].state));

    currentTimeMs = 15;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Pressed), static_cast<int>(buttons[1].state));
}

// Проверяет корректность вычисления задержки при переполнении счётчика millis().
void test_debounce_timer_handles_millis_rollover()
{
    const uint8_t pins[] = {25};
    ButtonConfigurator::configureButtons(ButtonsConfiguration(50, INPUT_PULLUP, pins));
    ButtonItem buttons[] = {ButtonItem(25)};

    currentTimeMs = UINT32_MAX - 20;
    setPinLevel(25, LOW);
    ButtonListener::updateState(buttons);

    currentTimeMs = 28;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Released), static_cast<int>(buttons[0].state));

    currentTimeMs = 29;
    ButtonListener::updateState(buttons);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonState::Pressed), static_cast<int>(buttons[0].state));
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ButtonTransition::Pressed), static_cast<int>(buttons[0].transition));
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_button_item_starts_released_with_no_transition);
    RUN_TEST(test_configurator_sets_mode_for_each_pin_and_stores_configuration);
    RUN_TEST(test_press_is_not_confirmed_before_debounce_delay);
    RUN_TEST(test_transition_is_reported_for_only_one_poll);
    RUN_TEST(test_bounce_restarts_debounce_timer);
    RUN_TEST(test_release_is_debounced);
    RUN_TEST(test_buttons_are_debounced_independently);
    RUN_TEST(test_debounce_timer_handles_millis_rollover);
    return UNITY_END();
}
