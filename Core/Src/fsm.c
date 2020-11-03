//
// Created by gaspar on 25/10/20.
//


#include <stdint-gcc.h>
#include "fsm.h"
#include "bsp.h"

static void no_action_handler(sm_context_t *context);
static void on_calendar_start_when_idle(sm_context_t *context);
static void on_calendar_stop_when_running(sm_context_t *context);
static void on_maxlevel_reached_when_running(sm_context_t *context);
static void on_startup_time_done_when_starting(sm_context_t *context);
/*
CALENDAR_START,
CALENDAR_STOP,
MAXLEVEL_REACHED,
LOW_RATE,
BATT_LEVEL_LOW,
 STARTUP_TIME_DONE,
ERROR_WAIT_TIME_COMPLETE);
 */


sm_declare_state_machine(
        pumpfsm,
        IDLE,
        (uint32_t) on_calendar_start_when_idle, // CALENDAR_START
        (uint32_t) no_action_handler,   // CALENDAR_STOP
        (uint32_t) no_action_handler,  // MAXLEVEL_REACHED
        (uint32_t) no_action_handler,   // LOW_RATE
        (uint32_t) no_action_handler,   // BATT_LEVEL_LOW
        (uint32_t) no_action_handler,   // STARTUP_TIME_DONE
        (uint32_t) no_action_handler,   // ERROR_WAIT_TIME_COMPLETE
        STARTING,
        (uint32_t) no_action_handler, // CALENDAR_START
        (uint32_t) no_action_handler,   // CALENDAR_STOP
        (uint32_t) no_action_handler,  // MAXLEVEL_REACHED
        (uint32_t) no_action_handler,   // LOW_RATE
        (uint32_t) no_action_handler,   // BATT_LEVEL_LOW
        (uint32_t) no_action_handler,   // STARTUP_TIME_DONE
        (uint32_t) no_action_handler,   // ERROR_WAIT_TIME_COMPLETE
        RUNNING,
        (uint32_t) no_action_handler, // CALENDAR_START
        (uint32_t) on_calendar_stop_when_running,   // CALENDAR_STOP
        (uint32_t) on_maxlevel_reached_when_running,  // MAXLEVEL_REACHED
        (uint32_t) no_action_handler,   // LOW_RATE
        (uint32_t) no_action_handler,   // BATT_LEVEL_LOW
        (uint32_t) no_action_handler,   // STARTUP_TIME_DONE
        (uint32_t) no_action_handler,   // ERROR_WAIT_TIME_COMPLETE
        ERROR,
        (uint32_t) no_action_handler, // CALENDAR_START
        (uint32_t) no_action_handler,   // CALENDAR_STOP
        (uint32_t) no_action_handler,  // MAXLEVEL_REACHED
        (uint32_t) no_action_handler,   // LOW_RATE
        (uint32_t) no_action_handler,   // BATT_LEVEL_LOW
        (uint32_t) no_action_handler,   // STARTUP_TIME_DONE
        (uint32_t) no_action_handler,   // ERROR_WAIT_TIME_COMPLETE
);

sm_define_handle_event(pumpfsm)

static void no_action_handler(sm_context_t *context) {
    debug2("no_action_handler");
}

static void on_calendar_start_when_idle(sm_context_t *context) {
    debug2("IDLE1");
    // start timer
    sm_set_state(context, STARTING);
}

static void on_calendar_stop_when_running(sm_context_t *context) {
    debug2("RUNNING");
    sm_set_state(context, IDLE);
}

static void on_maxlevel_reached_when_running(sm_context_t *context){
    debug2("RUNNING");
    sm_set_state(context, IDLE);
}

static void on_startup_time_done_when_starting(sm_context_t *context){
    sm_set_state(context, RUNNING);
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



