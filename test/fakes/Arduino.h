#pragma once

#include <stddef.h>
#include <stdint.h>

#define HIGH 1
#define LOW 0
#define INPUT 0
#define INPUT_PULLUP 2

unsigned long millis();
int digitalRead(uint8_t pin);
void pinMode(uint8_t pin, uint8_t mode);
