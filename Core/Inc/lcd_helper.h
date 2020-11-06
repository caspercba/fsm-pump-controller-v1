//
// Created by gaspar on 05/11/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H
#define FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H

#include <stdint-gcc.h>
#include "lcdhitachi.h"

void idle_show_mode1(uint8_t *time,
               uint8_t *timeNextStart) {
    LCD_setCursor(0,0);
    LCD_printf("IDLE");
    LCD_setCursor(0,8);
    LCD_printf(time);
    LCD_setCursor(1,0);
    LCD_printf("Next Start:");
    LCD_setCursor(1,8);
    LCD_printf(timeNextStart);
}

#endif //FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H
