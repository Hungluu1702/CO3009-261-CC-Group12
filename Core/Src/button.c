#include "button.h"

static GPIO_TypeDef *const btn_port[NO_OF_BUTTONS] =
    { BTN_1_GPIO_Port, BTN_2_GPIO_Port, BTN_3_GPIO_Port, BTN_4_GPIO_Port };
static const uint16_t btn_pin[NO_OF_BUTTONS] =
    { BTN_1_Pin, BTN_2_Pin, BTN_3_Pin, BTN_4_Pin };

static uint8_t  reg0[NO_OF_BUTTONS], reg1[NO_OF_BUTTONS], reg2[NO_OF_BUTTONS]; // 1 = pressed
static uint8_t  stable[NO_OF_BUTTONS];
static uint16_t hold[NO_OF_BUTTONS];
static uint8_t  pressed_flag[NO_OF_BUTTONS], long_flag[NO_OF_BUTTONS];

void button_init(void) { button_clear_all(); }

void button_clear_all(void) {
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        pressed_flag[i] = 0; long_flag[i] = 0; hold[i] = 0;
    }
}

void button_read(void) {
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        reg2[i] = reg1[i];
        reg1[i] = reg0[i];
        reg0[i] = (HAL_GPIO_ReadPin(btn_port[i], btn_pin[i]) == BUTTON_PRESSED_LEVEL);

        if (reg0[i] == reg1[i] && reg1[i] == reg2[i]) {      // 3 equal samples
            if (reg0[i] != stable[i]) {                      // level changed
                stable[i] = reg0[i];
                hold[i] = 0;
                if (stable[i]) pressed_flag[i] = 1;          // new press
            } else if (stable[i]) {                          // still held
                if (++hold[i] >= BUTTON_LONG_TICKS) {
                    long_flag[i] = 1;
                    hold[i] = BUTTON_LONG_TICKS - BUTTON_REPEAT_TICKS;
                }
            }
        }
    }
}

uint8_t isButtonPressed(uint8_t i) {
    if (i >= NO_OF_BUTTONS || !pressed_flag[i]) return 0;
    pressed_flag[i] = 0;
    return 1;
}

uint8_t isButtonLongPressed(uint8_t i) {
    if (i >= NO_OF_BUTTONS || !long_flag[i]) return 0;
    long_flag[i] = 0;
    return 1;
}
