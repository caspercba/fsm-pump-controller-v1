//
// Created by gaspar on 24/10/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_FSM_H
#define FSM_PUMP_CONTROLLER_V1_FSM_H

#include "bsp.h"

#define DEBUG 1

#define ST_IDLE                 0
#define ST_START_WAIT_2MIN      ST_IDLE + 1
#define ST_RUNNING              ST_START_WAIT_2MIN + 1
#define ST_ERROR                ST_RUNNING + 1
#define ST_TOTAL                ST_ERROR + 1

#define EV_BATT_LEVEL           0
#define EV_TANK_LEVEL           EV_BATT_LEVEL + 1
#define EV_PUMP_RATE            EV_TANK_LEVEL + 1
#define EV_BUTTON_PUSH          EV_PUMP_RATE + 1
#define EV_TIMER_TICK           EV_BUTTON_PUSH + 1
#define EV_TOTAL                EV_TIMER_TICK + 1

typedef struct {
    uint8_t id;
    uint32_t data;
    uint32_t epoch;
} str_event;


typedef struct {
    uint16_t tank_capacity_liters;
    uint16_t tank_min_liters;
    uint16_t pump_min_rate_liters_per_min;
    uint32_t batt_min_millivolts;
    uint8_t start_hour;
    uint8_t start_min;
    uint8_t stop_hour;
    uint8_t stop_min;
} str_fsm_configuration;

void fsm_enqueue_event(str_event);
void fsm_handle_events();
void fsm_set_config(str_fsm_configuration *config);

void debug1(const uint8_t *str);
void debug2(const uint8_t *str);

#endif //FSM_PUMP_CONTROLLER_V1_FSM_H
