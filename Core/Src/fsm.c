//
// Created by gaspar on 25/10/20.
//


#include <stdint-gcc.h>
#include <stdio.h>
#include <stdlib.h>
#include "fsm.h"
#include "bsp.h"
#include "lcd_helper.h"
#include "queue.h"

uint8_t currentState = ST_IDLE;

void printDebug(const uint8_t *func_name, uint32_t data);

static void no_action_handler(uint32_t data);
static void on_batt_level_x_idle(uint32_t data);
static void on_tank_level_x_idle(uint32_t data);
static void on_pump_rate_x_idle(uint32_t data);
static void on_button_push_x_idle(uint32_t data);
static void on_ev_calendar_x_idle(uint32_t data);
static void on_timertick_x_idle(uint32_t data);
static void on_batt_level_x_startup_batt(uint32_t data);
static void on_tank_level_x_startup_tank(uint32_t data);
static void on_timertick_x_startup_wait(uint32_t data);
static void on_timertick_x_running(uint32_t data);
static void on_calendar_x_running(uint32_t data);

static void on_batt_level_x_run_read(uint32_t data);
static void on_tank_level_x_run_read(uint32_t data);
static void on_timertick_x_read_rate(uint32_t data);


static void (*handler[ST_TOTAL][EV_TOTAL])(uint32_t data) = {
        // IDLE
        {on_batt_level_x_idle, on_tank_level_x_idle, on_pump_rate_x_idle, on_button_push_x_idle, on_ev_calendar_x_idle, on_timertick_x_idle},
        //
        {no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler},

        {no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler},

        {no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler},

        {no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler},

        {no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler},

        {no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler},

        {no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler},
        // ERROR
        {no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler, no_action_handler}

};

void fsm_init() {

}

void fsm_enqueue_event(str_event ev) {
    add(ev);
}
void fsm_handle_events() {
    str_event *next = getNext();

    if(next != NULL) {
        handler[currentState][next->id](next->data);
    }
}


/*
 ____ _____   ____  _____    _    ____    ____    _  _____ _____
/ ___|_   _| |  _ \| ____|  / \  |  _ \  | __ )  / \|_   _|_   _|
\___ \ | |   | |_) |  _|   / _ \ | | | | |  _ \ / _ \ | |   | |
 ___) || |   |  _ <| |___ / ___ \| |_| | | |_) / ___ \| |   | |
|____/ |_|   |_| \_\_____/_/   \_\____/  |____/_/   \_\_|   |_|
*/

/*
 ____ _____   ____  _____    _    ____    _____  _    _   _ _  __
/ ___|_   _| |  _ \| ____|  / \  |  _ \  |_   _|/ \  | \ | | |/ /
\___ \ | |   | |_) |  _|   / _ \ | | | |   | | / _ \ |  \| | ' /
 ___) || |   |  _ <| |___ / ___ \| |_| |   | |/ ___ \| |\  | . \
|____/ |_|   |_| \_\_____/_/   \_\____/    |_/_/   \_\_| \_|_|\_\

 */


/*
 ____ _____  __        ___    ___ _____   ____  __  __ ___ _   _
/ ___|_   _| \ \      / / \  |_ _|_   _| |___ \|  \/  |_ _| \ | |
\___ \ | |    \ \ /\ / / _ \  | |  | |     __) | |\/| || ||  \| |
 ___) || |     \ V  V / ___ \ | |  | |    / __/| |  | || || |\  |
|____/ |_|      \_/\_/_/   \_\___| |_|   |_____|_|  |_|___|_| \_|

 */

/*
 ____  _   _ _   _ _   _ ___ _   _  ____
|  _ \| | | | \ | | \ | |_ _| \ | |/ ___|
| |_) | | | |  \| |  \| || ||  \| | |  _
|  _ <| |_| | |\  | |\  || || |\  | |_| |
|_| \_\\___/|_| \_|_| \_|___|_| \_|\____|

 */
static void no_action_handler(uint32_t data) {
    debug2((uint8_t *)data);
}


/*
 ___ ____  _     _____
|_ _|  _ \| |   | ____|
 | || | | | |   |  _|
 | || |_| | |___| |___
|___|____/|_____|_____|
*/

static void on_batt_level_x_idle(uint32_t data) {
    printDebug(__func__, data);
}
static void on_tank_level_x_idle(uint32_t data) {
    printDebug(__func__, data);
}
static void on_pump_rate_x_idle(uint32_t data) {
    printDebug(__func__, data);
}
static void on_button_push_x_idle(uint32_t data) {
    printDebug(__func__, data);
}
static void on_ev_calendar_x_idle(uint32_t data) {
    printDebug(__func__, data);
}
static void on_timertick_x_idle(uint32_t data) {
    printDebug(__func__, data);
}

static void on_batt_level_x_startup_batt(uint32_t data) {
    printDebug(__func__, data);
}
static void on_tank_level_x_startup_tank(uint32_t data) {
    printDebug(__func__, data);
}
static void on_timertick_x_startup_wait(uint32_t data) {
}
static void on_timertick_x_running(uint32_t data) {
}
static void on_calendar_x_running( uint32_t data) {
}
static void on_batt_level_x_run_read(uint32_t data) {
}
static void on_tank_level_x_run_read(uint32_t data) {
}
static void on_timertick_x_read_rate(uint32_t data) {
}

void printDebug(const uint8_t *func_name, uint32_t data) {
    printf("%s data: %lu\r\n", func_name, data);
}

void debug2(const uint8_t *str) {
    if(DEBUG/* && str != NULL*/) {
        LCD_writeLine2(str);
    }
}
void debug1(const uint8_t *str) {
    if(DEBUG) {
        LCD_writeLine1(str);
    }
}



