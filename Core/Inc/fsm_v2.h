//
// Created by gaspar on 03/11/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_FSM_V2_H
#define FSM_PUMP_CONTROLLER_V1_FSM_V2_H

#include <stdint-gcc.h>
#include "bsp.h"

#define ST_IDLE                         0
#define ST_STARTUP_READ_BATT            ST_IDLE + 1
#define ST_START_READ_TANK              ST_STARTUP_READ_BATT + 1
#define ST_START_WAIT_2MIN              ST_START_READ_TANK + 1
#define ST_RUNNING                      ST_START_WAIT_2MIN + 1
#define ST_RUN_READ_TANK                ST_RUNNING + 1
#define ST_RUN_READ_BATT                ST_RUN_READ_TANK + 1
#define ST_RUN_READ_RATE                ST_RUN_READ_BATT + 1
#define ST_ERROR                        ST_RUN_READ_RATE + 1
#define ST_TOTAL                        ST_ERROR + 1

#define EV_BATT_OK              0
#define EV_BATT_NOT_OK          EV_BATT_OK + 1
#define EV_TANK_FULL            EV_BATT_NOT_OK + 1
#define EV_TANK_NEEDS_WATER     EV_TANK_FULL + 1
#define EV_PUMP_RATE_OK         EV_TANK_NEEDS_WATER + 1
#define EV_PUMP_RATE_NOT_OK     EV_PUMP_RATE_OK + 1
#define EV_START_PUSHED         EV_PUMP_RATE_NOT_OK + 1
#define EV_STOP_PUSHED          EV_START_PUSHED + 1
#define EV_AUTO_PUSHED          EV_STOP_PUSHED + 1
#define EV_CALENDAR_START       EV_AUTO_PUSHED + 1
#define EV_CALENDAR_STOP        EV_CALENDAR_START + 1
#define EV_TIMER_2MIN_FINISH    EV_CALENDAR_STOP + 1
#define EV_TOTAL                EV_TIMER_2MIN_FINISH + 1


typedef struct {
    uint32_t tankLitersPerMilliVolt;
    uint16_t tankLiters;
    uint32_t tankLitersMin;
    uint16_t pumpMinRateLitersMin;
    uint8_t refresh_rate_per_sec;
    uint32_t batt_min_level;
} str_config;

typedef struct {
    uint8_t value;
} str_fsm_event;

typedef struct {
    uint8_t value;
} str_fsm_state;

typedef struct {
    str_fsm_state state;
    str_fsm_event event;
    str_fsm_state *(*func) (str_config* config, void * data);
} str_fsm_transition;


typedef struct {
    str_fsm_state *state;
    str_fsm_state *states[ST_TOTAL];
    str_config *config;
} str_fsm;

str_fsm_state * st_idle_x_ev_calendar_start(str_fsm *fsm, void * data);

void timer_2min_start();
str_fsm_event* ev_get();
void ev_send(str_fsm_event* event);


#endif //FSM_PUMP_CONTROLLER_V1_FSM_V2_H
