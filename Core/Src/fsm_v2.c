//
// Created by gaspar on 03/11/20.
//

#include "fsm_v2.h"

str_fsm_transition transitions[] = {
        {ST_IDLE, EV_CALENDAR_START, st_idle_x_ev_calendar_start}
};

str_fsm_state * st_idle_x_ev_calendar_start(str_fsm *fsm, void * data) {
    timer_2min_start();
    return fsm->states[ST_STARTUP_READ_BATT];
}

