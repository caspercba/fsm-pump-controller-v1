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

sm_declare_states(pumpfsm,
                  IDLE,
                  STARTING,
                  RUNNING,
                  ERRO);
sm_declare_events(pumpfsm,
                  CALENDAR_START,
                  CALENDAR_STOP,
                  MAXLEVEL_REACHED,
                  LOW_RATE,
                  BATT_LEVEL_LOW,
                  STARTUP_TIME_DONE,
                  ERROR_WAIT_TIME_COMPLETE);

sm_declare_handle_event(pumpfsm);

void debug1(const uint8_t *str);
void debug2(const uint8_t *str);

#endif //FSM_PUMP_CONTROLLER_V1_FSM_H
