//
// Created by gaspar on 25/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "fsm.h"
#include "bsp.h"
#include "lcd_helper.h"
#include "queue.h"
#include "menu.h"

uint8_t currentState = ST_IDLE;
str_fsm_configuration *config = NULL;

str_event last_events[EV_TOTAL];
uint32_t current_rate = 0;
uint8_t bypass_checks = FALSE;
uint8_t * error_msg = "";



void printDebug(const uint8_t *func_name, str_event * ev);
uint8_t should_start();
uint8_t check_pump_rate();
uint8_t check_batt_level();
uint8_t check_time();

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

static void on_button_push_x_config(str_event* ev );
static void on_timertick_x_config(str_event* ev );




static void (*handler[ST_TOTAL][EV_TOTAL])(str_event *ev) = {
        // IDLE
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_idle, on_timertick_x_idle},
        // ST_START_WAIT_2MIN
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_startup, on_timertick_x_startup},
        // ST_RUNNING
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_running, on_timertick_x_running},
        // ST_ERROR
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_error, on_timertick_x_error},
        // ST_CONFIG
        {on_batt_level, on_tank_level, on_pump_rate, on_button_push_x_config, on_timertick_x_config}
};

void fsm_set_config(str_fsm_configuration *cfg) {
    config = cfg;
    MENU_init(config);
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
#ifdef DEBUG
        printf("ST: %02d, EV: %01d, data: %09d\r\n", currentState, next->id, next->data);
#endif
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
    switch (ev->data) {
        case BTN_MODE:
            switch_mode();
            break;
        case BTN_FORCE_START:
            bypass_checks = TRUE;
            currentState = ST_RUNNING;
            break;
        case BTN_CONFIG:
            currentState = ST_CONFIG;
            break;
    }
}
static void on_timertick_x_idle(str_event *ev) {
    bypass_checks = FALSE;
    error_msg = "";

    if(check_batt_level() == TRUE && check_time() == TRUE && config->auto_enabled == TRUE) {
        SYSTEM_pump_set(TRUE);
        currentState = ST_START_WAIT_2MIN;
    } else {
        SYSTEM_pump_set(FALSE);
    }

    uint32_t batt = last_events[EV_BATT_LEVEL].data;
    uint32_t tank = last_events[EV_TANK_LEVEL].data;
    uint32_t tank_percent = min((tank * 100) / config->tank_capacity_liters, 100);

    LCDH_idle(ev->data,
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
    switch (ev->data) {
        case BTN_MODE:
            switch_mode();
            break;
        case BTN_FORCE_START:
            currentState = ST_IDLE;
            bypass_checks = FALSE;
            break;
    }
}

time_t time_when_entered_startup = 0;

static void on_timertick_x_startup(str_event *ev) {
    time_t now = SYSTEM_getEpoch();
    //time ( &now );
    time_t wait_time = now - time_when_entered_startup;
    if(time_when_entered_startup == 0) {
        time_when_entered_startup = now;
    } else if(wait_time > config->startup_wait_secs) {

        if(check_pump_rate()) {
            if(check_batt_level() && check_time()) {
                currentState = ST_RUNNING;
                time_when_entered_startup = 0;
            } else {
                currentState = ST_IDLE;
                time_when_entered_startup = 0;
            }
        } else {
            time_when_entered_startup = 0;
            currentState = ST_ERROR;
            error_msg = "Check pump";
        }
    } else {
        // wait till 2 min is done
        LCDH_waiting(wait_time, last_events[EV_PUMP_RATE].data);
    }
}



/*
 ____  _   _ _   _ _   _ ___ _   _  ____
|  _ \| | | | \ | | \ | |_ _| \ | |/ ___|
| |_) | | | |  \| |  \| || ||  \| | |  _
|  _ <| |_| | |\  | |\  || || |\  | |_| |
|_| \_\\___/|_| \_|_| \_|___|_| \_|\____|

 */

static void on_button_push_x_running(str_event *ev) {
    switch(ev->data) {
        case BTN_MODE:
            switch_mode();
            break;
        case BTN_FORCE_START:
            SYSTEM_pump_set(FALSE);
            currentState = ST_IDLE;
            break;
    }
}
static void on_timertick_x_running(str_event *ev) {

    if(check_pump_rate() == TRUE || bypass_checks) {
        if((check_batt_level() == TRUE && check_time() == TRUE) || bypass_checks) {
            SYSTEM_pump_set(TRUE);
            uint32_t tank_percent =
                    min((last_events[EV_TANK_LEVEL].data * 100) / config->tank_capacity_liters, 100);
            LCDH_running(SYSTEM_getTimeDate(),
                         last_events[EV_TANK_LEVEL].data,
                         last_events[EV_BATT_LEVEL].data,
                         tank_percent,
                         last_events[EV_PUMP_RATE].data);
        } else {
            currentState = ST_IDLE;
            SYSTEM_pump_set(FALSE);
        }
    } else {
        SYSTEM_pump_set(FALSE);
        currentState = ST_ERROR;
        error_msg = "Check pump!";
    }
}




/*
 _____ ____  ____   ___  ____
| ____|  _ \|  _ \ / _ \|  _ \
|  _| | |_) | |_) | | | | |_) |
| |___|  _ <|  _ <| |_| |  _ <
|_____|_| \_\_| \_\\___/|_| \_\
 */

static void on_button_push_x_error(str_event *ev) {
    switch(ev->data) {
        case BTN_FORCE_START:
            currentState = ST_IDLE;
            break;
    }
}
static void on_timertick_x_error(str_event *ev) {
    SYSTEM_pump_set(FALSE);
    bypass_checks = FALSE;
    LCDH_error(error_msg);
}

/*
  ____             __ _
 / ___|___  _ __  / _(_) __ _
| |   / _ \| '_ \| |_| |/ _` |
| |__| (_) | | | |  _| | (_| |
 \____\___/|_| |_|_| |_|\__, |
                        |___/
 */



static void on_button_push_x_config(str_event* ev ) {
    if(MENU_handle_button(ev->data) == FALSE) {
        SYSTEM_save(config);
        currentState = ST_IDLE;
    }
}
static void on_timertick_x_config(str_event* ev ) {
    MENU_tick();
}


/*
 ____ _____ _   _ _____ _____
/ ___|_   _| | | |  ___|  ___|
\___ \ | | | | | | |_  | |_
 ___) || | | |_| |  _| |  _|
|____/ |_|  \___/|_|   |_|
 */

uint8_t check_pump_rate() {
    if(last_events[EV_PUMP_RATE].data > config->pump_min_rate_liters_per_min
    || bypass_checks) {
        return TRUE;
    }
    return FALSE;
}


uint8_t check_batt_level() {
    if(last_events[EV_BATT_LEVEL].data > config->batt_min_millivolts
    || bypass_checks) {
        return TRUE;
    }
    return FALSE;
}


uint8_t check_time() {
    struct tm *time = SYSTEM_getTimeDate();
    if((time->tm_hour > config->start_hour &&
    time->tm_min > config->start_min &&
    time->tm_hour < config->stop_hour &&
    time->tm_min < config->stop_min) ||
    bypass_checks) {
        return TRUE;
    }
    return FALSE;
}

uint8_t should_start() {
    return FALSE;
}

void printDebug(const uint8_t *func_name, str_event * ev) {
    printf("%s eventID: %d, data: %lu, epoch: %lu\r\n", func_name, ev->id, ev->data, ev->epoch);
}

static uint32_t helper_pump_rate() {

}

