#pragma once

#include <Arduino.h>

void _setup_gpio();
void _post_setup_gpio();
void _setBrightness(uint8_t brightval);
void InputHandler(void);
int getBattery();
void powerOff();
void checkReboot();
