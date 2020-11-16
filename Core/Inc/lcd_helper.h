//
// Created by gaspar on 05/11/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H
#define FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H

#include <stdint-gcc.h>
#include "lcdhitachi.h"
#include <time.h>

void show_idle(struct tm * time, uint32_t tank, uint32_t batt, uint32_t tank_percent);
void show_running();

#endif //FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H
