//
// Created by gaspar on 06/11/20.
// https://en.wikipedia.org/wiki/Circular_buffer
//

#include <limits.h>
#include <stdint-gcc.h>
#include <stdio.h>
#include "queue.h"

#define CIRCULAR_BUFF_SIZE 10


str_event event_buff[CIRCULAR_BUFF_SIZE];
str_event *read = NULL;
str_event *write = event_buff;
uint8_t data_size = 0;

void queue_add(str_event event) {
    if(write >= event_buff + CIRCULAR_BUFF_SIZE) {
        write = event_buff;
    }
    write->data = event.data;
    write->epoch = event.epoch;
    write->id = event.id;
    if(data_size == 0) read=write;
    write++;
    data_size++;
    if(data_size > CIRCULAR_BUFF_SIZE) data_size = CIRCULAR_BUFF_SIZE;
}

str_event *queue_getNext() {
    if(data_size < 1) return NULL;

    if(read >= event_buff + CIRCULAR_BUFF_SIZE) read = event_buff;
    data_size--;

    printf("QUEUE: %02d\r\n", data_size);
    str_event *result = read;
    read++;
    if(read >= event_buff + CIRCULAR_BUFF_SIZE) read = event_buff;
    printf("queue pending events: %d\r\n", data_size);
    return result;
}