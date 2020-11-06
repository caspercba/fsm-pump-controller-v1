//
// Created by gaspar on 06/11/20.
// https://en.wikipedia.org/wiki/Circular_buffer
//

#ifndef FSM_PUMP_CONTROLLER_V1_QUEUE_H
#define FSM_PUMP_CONTROLLER_V1_QUEUE_H

#include "fsm.h"

void add(str_event event);
str_event *getNext();

#endif //FSM_PUMP_CONTROLLER_V1_QUEUE_H
