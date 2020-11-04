//
// Created by gaspar on 24/10/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_FSM_H
#define FSM_PUMP_CONTROLLER_V1_FSM_H

#define DEBUG 1

#include "sm_framework.h"

typedef struct {
    uint32_t tankLitersPerMilliVolt;
    uint16_t tankLiters;
    uint16_t tankLitersMin;
    uint16_t pumpMinRateLitersMin;
    uint8_t refresh_rate_per_sec;
} CONFIGURATION;

/*
 *
 * #define ST_IDLE                         0
#define ST_STARTUP_READ_BATT            ST_IDLE + 1
#define ST_START_READ_TANK              ST_STARTUP_READ_BATT + 1
#define ST_START_WAIT_2MIN              ST_START_READ_TANK + 1
#define ST_RUNNING                      ST_START_WAIT_2MIN + 1
#define ST_RUN_READ_TANK                ST_RUNNING + 1
#define ST_RUN_READ_BATT                ST_RUN_READ_TANK + 1
#define ST_RUN_READ_RATE                ST_RUN_READ_BATT + 1
#define ST_ERROR                        ST_RUN_READ_RATE + 1
#define ST_TOTAL                        ST_ERROR + 1
 */

sm_declare_states(pumpfsm,
                  ST_IDLE,
                  ST_STARTUP_READ_BATT,
                  ST_START_READ_TANK,
                  ST_START_WAIT_2MIN,
                  ST_RUNNING,
                  ST_RUN_READ_TANK,
                  ST_RUN_READ_BATT,
                  ST_RUN_READ_RATE,
                  ST_ERROR);


sm_declare_events(pumpfsm,
                  EV_BATT_LEVEL,
                  EV_TANK_LEVEL,
                  EV_PUMP_RATE,
                  EV_BUTTON_PUSH,
                  EV_CALENDAR,
                  EV_TIMER_TICK
                  );

sm_declare_handle_event(pumpfsm);

void debug1(const uint8_t *str);
void debug2(const uint8_t *str);

#endif //FSM_PUMP_CONTROLLER_V1_FSM_H
