//
// Created by gaspar on 03/11/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_FSM_V2_H
#define FSM_PUMP_CONTROLLER_V1_FSM_V2_H

#include <stdint-gcc.h>
#include "bsp.h"

#define ST_IDLE             0
#define ST_STARTUP          ST_IDLE + 1
#define ST_READING_BATT     ST_STARTUP + 1
#define ST_READING_TANK     ST_READING_BATT + 1
#define ST_RUNNING          ST_READING_TANK + 1
#define ST_ERROR            ST_RUNNING + 1
#define ST_TOTAL            ST_ERROR + 1

#define EV_MEASURED_BATT        0
#define EV_MEASURED_TANK        EV_MEASURED_BATT + 1
#define EV_REQ_MEASURE          EV_MEASURED_TANK + 1
#define EV_START_PUSHED         EV_REQ_MEASURE + 1
#define EV_STOP_PUSHED          EV_START_PUSHED + 1
#define EV_AUTO_PUSHED          EV_STOP_PUSHED + 1
#define EV_CALENDAR_START       EV_AUTO_PUSHED + 1
#define EV_CALENDAR_STOP        EV_CALENDAR_START + 1
#define EV_TIMER_5MIN_FINISH    EV_CALENDAR_STOP + 1


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
    void (*func) (str_config* config, void * data);
} str_fsm_transition;


typedef struct {
    str_fsm_state *state;
    str_fsm_state *states[ST_TOTAL];
    str_config *config;
} str_fsm;

void st_idle_x_ev_calendar_start(str_fsm *fsm, void * data);
void st_idle_x_ev_start_pushed(void * data);
void any_x_ev_auto_pushed(void * data);
void st_startup_x_ev_timer_5min_finish(str_fsm *fsm, void * data);
void st_reading_batt_x_ev_measured_batt(str_fsm *fsm, void *data);
void st_reading_batt_x_ev_measured_tank(str_fsm *fsm, void *data);

void timer_5min_start();

str_fsm_event* ev_get() {
}

void ev_send(str_fsm_event* event);
void st_set(str_fsm* fsm, uint8_t ST) {
    fsm->state = fsm->states[ST];
}


str_fsm_transition transitions[] = {
        {ST_IDLE, EV_CALENDAR_START, st_idle_x_ev_calendar_start},
        {ST_IDLE, EV_START_PUSHED, st_idle_x_ev_start_pushed},
        {ST_IDLE, EV_AUTO_PUSHED, any_x_ev_auto_pushed},
        {ST_STARTUP, EV_TIMER_5MIN_FINISH, st_startup_x_ev_timer_5min_finish},
        {ST_READING_BATT, EV_MEASURED_BATT, st_reading_batt_x_ev_measured_batt},
        { ST_READING_TANK, EV_MEASURED_TANK, st_reading_batt_x_ev_measured_tank },
        {}
};



void st_idle_x_ev_calendar_start(str_fsm *fsm, void * data) {
    timer_5min_start();
    st_set(fsm, ST_STARTUP);
}

void st_startup_x_ev_timer_5min_finish(str_fsm *fsm, void *data) {
    SYSTEM_get_tank_level();
    st_set(fsm, ST_READING_BATT);
}

void st_reading_batt_x_ev_measured_batt(str_fsm *fsm, void* data) {
    if(SYSTEM_get_batt_level() > fsm->config->batt_min_level) {
        SYSTEM_get_tank_level();
        st_set(fsm, ST_READING_TANK);
    } else {
        st_set(fsm, ST_IDLE);
    }
}

void st_reading_batt_x_ev_measured_tank(str_fsm *fsm, void *data) {
    if(SYSTEM_get_tank_level() < fsm->config->tankLitersMin) {
        st_set(fsm, ST_RUNNING);
    } else {
        st_set(fsm, ST_IDLE);
    }
}






void run() {

}

#endif //FSM_PUMP_CONTROLLER_V1_FSM_V2_H
