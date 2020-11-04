//
// Created by gaspar on 25/10/20.
//


#include <stdint-gcc.h>
#include "fsm.h"
#include "bsp.h"

static void no_action_handler(sm_context_t *context, void *data);
static void on_calendar_start_when_idle(sm_context_t *context);
static void on_calendar_stop_when_running(sm_context_t *context);
static void on_maxlevel_reached_when_running(sm_context_t *context);
static void on_startup_time_done_when_starting(sm_context_t *context);
/*
                  ST_IDLE,
                  ST_STARTUP_READ_BATT,
                  ST_START_READ_TANK,
                  ST_START_WAIT_2MIN,
                  ST_RUNNING,
                  ST_RUN_READ_TANK,
                  ST_RUN_READ_BATT,
                  ST_RUN_READ_RATE,
                  ST_ERROR);

                  EV_BATT_LEVEL,
                  EV_TANK_LEVEL,
                  EV_PUMP_RATE,
                  EV_BUTTON_PUSH,
                  EV_CALENDAR,
                  EV_TIMER_TICK
 */


sm_declare_state_machine(
        pumpfsm,
        ST_IDLE,
        (uint32_t) no_action_handler,   // EV_BATT_LEVEL
        (uint32_t) no_action_handler,   // EV_TANK_LEVEL
        (uint32_t) no_action_handler,   // EV_PUMP_RATE
        (uint32_t) no_action_handler,   // EV_BUTTON_PUSH
        (uint32_t) no_action_handler,   // EV_CALENDAR
        (uint32_t) no_action_handler,   // EV_TIMER_TICK
        ST_STARTUP_READ_BATT,
        (uint32_t) no_action_handler,   // EV_BATT_LEVEL
        (uint32_t) no_action_handler,   // EV_TANK_LEVEL
        (uint32_t) no_action_handler,   // EV_PUMP_RATE
        (uint32_t) no_action_handler,   // EV_BUTTON_PUSH
        (uint32_t) no_action_handler,   // EV_CALENDAR
        (uint32_t) no_action_handler,   // EV_TIMER_TICK
        ST_START_READ_TANK,
        (uint32_t) no_action_handler,   // EV_BATT_LEVEL
        (uint32_t) no_action_handler,   // EV_TANK_LEVEL
        (uint32_t) no_action_handler,   // EV_PUMP_RATE
        (uint32_t) no_action_handler,   // EV_BUTTON_PUSH
        (uint32_t) no_action_handler,   // EV_CALENDAR
        (uint32_t) no_action_handler,   // EV_TIMER_TICK
        ST_START_WAIT_2MIN,
        (uint32_t) no_action_handler,   // EV_BATT_LEVEL
        (uint32_t) no_action_handler,   // EV_TANK_LEVEL
        (uint32_t) no_action_handler,   // EV_PUMP_RATE
        (uint32_t) no_action_handler,   // EV_BUTTON_PUSH
        (uint32_t) no_action_handler,   // EV_CALENDAR
        (uint32_t) no_action_handler,   // EV_TIMER_TICK
        ST_RUNNING,
        (uint32_t) no_action_handler,   // EV_BATT_LEVEL
        (uint32_t) no_action_handler,   // EV_TANK_LEVEL
        (uint32_t) no_action_handler,   // EV_PUMP_RATE
        (uint32_t) no_action_handler,   // EV_BUTTON_PUSH
        (uint32_t) no_action_handler,   // EV_CALENDAR
        (uint32_t) no_action_handler,   // EV_TIMER_TICK
        ST_RUN_READ_TANK,
        (uint32_t) no_action_handler,   // EV_BATT_LEVEL
        (uint32_t) no_action_handler,   // EV_TANK_LEVEL
        (uint32_t) no_action_handler,   // EV_PUMP_RATE
        (uint32_t) no_action_handler,   // EV_BUTTON_PUSH
        (uint32_t) no_action_handler,   // EV_CALENDAR
        (uint32_t) no_action_handler,   // EV_TIMER_TICK
        ST_RUN_READ_BATT,
        (uint32_t) no_action_handler,   // EV_BATT_LEVEL
        (uint32_t) no_action_handler,   // EV_TANK_LEVEL
        (uint32_t) no_action_handler,   // EV_PUMP_RATE
        (uint32_t) no_action_handler,   // EV_BUTTON_PUSH
        (uint32_t) no_action_handler,   // EV_CALENDAR
        (uint32_t) no_action_handler,   // EV_TIMER_TICK
        ST_RUN_READ_RATE,
        (uint32_t) no_action_handler,   // EV_BATT_LEVEL
        (uint32_t) no_action_handler,   // EV_TANK_LEVEL
        (uint32_t) no_action_handler,   // EV_PUMP_RATE
        (uint32_t) no_action_handler,   // EV_BUTTON_PUSH
        (uint32_t) no_action_handler,   // EV_CALENDAR
        (uint32_t) no_action_handler,   // EV_TIMER_TICK
        ST_ERROR
);

sm_define_handle_event(pumpfsm)

static void no_action_handler(sm_context_t *context, void* data) {
    debug2((char*)data);
}

static void on_calendar_start_when_idle(sm_context_t *context) {
    debug2("IDLE1");
    // start timer
    //sm_set_state(context, STARTING);
}

static void on_calendar_stop_when_running(sm_context_t *context) {
    debug2("RUNNING");
    //sm_set_state(context, IDLE);
}

static void on_maxlevel_reached_when_running(sm_context_t *context){
    debug2("RUNNING");
    //sm_set_state(context, IDLE);
}

static void on_startup_time_done_when_starting(sm_context_t *context){
    //sm_set_state(context, RUNNING);
}

void debug2(const uint8_t *str) {
    if(DEBUG) {
        LCD_writeLine2(str);
    }
}
void debug1(const uint8_t *str) {
    if(DEBUG) {
        LCD_writeLine1(str);
    }
}



