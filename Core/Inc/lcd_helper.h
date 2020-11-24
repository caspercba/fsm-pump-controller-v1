//
// Created by gaspar on 05/11/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H
#define FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H

#include <stdint-gcc.h>
#include "lcdhitachi.h"
#include <time.h>

#define MODE0       0
#define MODE1       MODE0 + 1
//#define MODE2       MODE1 + 1
//#define NMODES      MODE2 + 1
#define NMODES      MODE1 + 1

void switch_mode(void);

void LCDH_idle(struct tm * time_now, uint32_t tank, uint32_t batt, uint32_t tank_percent);
void LCDH_waiting(struct tm * wait_time, uint32_t pump_rate);
void LCDH_running(struct tm * time_now, uint32_t tank, uint32_t batt, uint32_t tank_percent, uint32_t pump_rate);
void LCDH_error(uint8_t * msg);

#endif //FSM_PUMP_CONTROLLER_V1_LCD_HELPER_H
