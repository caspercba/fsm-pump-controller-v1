//
// Created by gaspar on 24/11/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_MENU_H
#define FSM_PUMP_CONTROLLER_V1_MENU_H


#include "fsm.h"

#define ST_CFG_START_HOUR       0
#define ST_CFG_START_MIN        ST_CFG_START_HOUR + 1
#define ST_CFG_STOP_HOUR        ST_CFG_START_MIN + 1
#define ST_CFG_STOP_MIN         ST_CFG_STOP_HOUR + 1
#define ST_CFG_MAX_LITERS       ST_CFG_STOP_MIN + 1
#define ST_CFG_MIN_LITERS       ST_CFG_MAX_LITERS + 1
#define ST_CFG_MIN_PUMP_RATE    ST_CFG_MIN_LITERS + 1
#define ST_CFG_NUM_ST           ST_CFG_MIN_PUMP_RATE + 1


void MENU_init(str_fsm_configuration *cfg);
uint8_t MENU_handle_button(uint32_t button);
void MENU_tick();

#endif //FSM_PUMP_CONTROLLER_V1_MENU_H
