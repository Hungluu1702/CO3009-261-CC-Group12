#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"

#define NO_OF_BUTTONS        4
#define BUTTON_PRESSED_LEVEL GPIO_PIN_RESET  // from T08: level read while pressed
#define BUTTON_LONG_TICKS    100             // 100 x 10 ms = 1 s
#define BUTTON_REPEAT_TICKS  20              // 20 x 10 ms = 200 ms

void    button_init(void);
void    button_read(void);                   // run as a task every 10 ms
uint8_t isButtonPressed(uint8_t i);          // 1 once per press, then cleared
uint8_t isButtonLongPressed(uint8_t i);      // 1 at 1 s, then every 200 ms
void    button_clear_all(void);              // use on every mode change

#endif
