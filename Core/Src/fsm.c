//
// Created by gaspar on 25/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "fsm.h"
#include "bsp.h"
#include "lcd_helper.h"
#include "queue.h"

uint8_t currentState = ST_IDLE;
str_fsm_configuration *config = NULL;

str_event last_events[EV_TOTAL];
uint32_t current_rate = 0;



void printDebug(const uint8_t *func_name, str_event * ev);
uint8_t should_start();

static void no_action_handler(str_event *ev);
static void on_batt_level(str_event *ev);
static void on_tank_level(str_event *ev);
static void on_pump_rate(str_event *ev);


static void on_button_push_x_idle(str_event *ev);
static void on_timertick_x_idle(str_event *ev);

static void on_button_push_x_startup(str_event *ev);
static void on_timertick_x_startup(str_event *ev);

static void on_button_push_x_running(str_event *ev);
static void on_timertick_x_running(str_event *ev);

static void on_button_push_x_error(str_event *ev);
static void on_timertick_x_error(str_event *ev);



static void (*handler[ST_TOTAL][EV_TOTAL])(str_event *ev) = {
        // IDLE
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_idle, on_timertick_x_idle},
        // ST_START_WAIT_2MIN
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_startup, on_timertick_x_startup},
        // ST_RUNNING
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_running, on_timertick_x_running},
        // ST_ERROR
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_error, on_timertick_x_error}
};

void fsm_set_config(str_fsm_configuration *cfg) {
    config = cfg;
}

void fsm_enqueue_event(str_event ev) {
    if(ev.id == EV_TANK_LEVEL && last_events[ev.id].data > 0) {
        current_rate = (ev.data - last_events[ev.id].data) * 60 / (ev.epoch - last_events[ev.id].epoch);
    }
    last_events[ev.id] = ev;
    queue_add(ev);
}
void fsm_handle_events() {
    str_event *next = queue_getNext();

    if(next != NULL) {
        handler[currentState][next->id](next);
    }
}


static void no_action_handler(str_event *ev) {}




/*
 ___ ____  _     _____
|_ _|  _ \| |   | ____|
 | || | | | |   |  _|
 | || |_| | |___| |___
|___|____/|_____|_____|
*/

static void on_batt_level(str_event *ev) {
    last_events[EV_BATT_LEVEL] = *ev;
}
static void on_tank_level(str_event *ev) {
    last_events[EV_TANK_LEVEL] = *ev;
}
static void on_pump_rate(str_event *ev) {
    last_events[EV_PUMP_RATE] = *ev;
}
static void on_button_push_x_idle(str_event *ev) {

}
static void on_timertick_x_idle(str_event *ev) {

    if(should_start()) {
        SYSTEM_pump_set(TRUE);
        currentState = ST_START_WAIT_2MIN;
    }

    uint32_t batt = last_events[EV_BATT_LEVEL].data;
    uint32_t tank = last_events[EV_TANK_LEVEL].data;
    uint32_t tank_percent = min((tank * 100) / config->tank_capacity_liters, 100);

    show_idle(ev->data,
              tank,
              batt,
              tank_percent);
}


/*
 ____ _____  __        ___    ___ _____   ____  __  __ ___ _   _
/ ___|_   _| \ \      / / \  |_ _|_   _| |___ \|  \/  |_ _| \ | |
\___ \ | |    \ \ /\ / / _ \  | |  | |     __) | |\/| || ||  \| |
 ___) || |     \ V  V / ___ \ | |  | |    / __/| |  | || || |\  |
|____/ |_|      \_/\_/_/   \_\___| |_|   |_____|_|  |_|___|_| \_|

 */

static void on_button_push_x_startup(str_event *ev) {

}
static void on_timertick_x_startup(str_event *ev) {

}



/*
 ____  _   _ _   _ _   _ ___ _   _  ____
|  _ \| | | | \ | | \ | |_ _| \ | |/ ___|
| |_) | | | |  \| |  \| || ||  \| | |  _
|  _ <| |_| | |\  | |\  || || |\  | |_| |
|_| \_\\___/|_| \_|_| \_|___|_| \_|\____|

 */

static void on_button_push_x_running(str_event *ev) {

}
static void on_timertick_x_running(str_event *ev) {

}




/*
 _____ ____  ____   ___  ____
| ____|  _ \|  _ \ / _ \|  _ \
|  _| | |_) | |_) | | | | |_) |
| |___|  _ <|  _ <| |_| |  _ <
|_____|_| \_\_| \_\\___/|_| \_\
 */

static void on_button_push_x_error(str_event *ev) {

}
static void on_timertick_x_error(str_event *ev) {

}




/*
 ____ _____ _   _ _____ _____
/ ___|_   _| | | |  ___|  ___|
\___ \ | | | | | | |_  | |_
 ___) || | | |_| |  _| |  _|
|____/ |_|  \___/|_|   |_|
 */

uint8_t should_start() {
    return FALSE;
}

void printDebug(const uint8_t *func_name, str_event * ev) {
    printf("%s eventID: %d, data: %lu, epoch: %lu\r\n", func_name, ev->id, ev->data, ev->epoch);
}

static uint32_t helper_pump_rate() {

}

