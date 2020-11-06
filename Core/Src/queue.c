//
// Created by gaspar on 06/11/20.
// https://en.wikipedia.org/wiki/Circular_buffer
//

#include <limits.h>
#include <stdint-gcc.h>
#include <stdio.h>
#include "queue.h"

#define B_SIZE 10

str_event event_buff[B_SIZE];
str_event *read = NULL;
str_event *write = event_buff;
uint8_t data_size = 0;

void add(str_event event) {
    if(write >= event_buff + B_SIZE) {
        write = event_buff;
    }
    write->data = event.data;
    write->id = event.id;
    if(data_size == 0) read=write;
    write++;
    data_size++;
    if(data_size > B_SIZE) data_size = B_SIZE;
}

str_event *getNext() {
    if(data_size < 1) return NULL;

    if(read >= event_buff + B_SIZE) read = event_buff;
    data_size--;
    printf("queue pending events: %d\r\n", data_size);
    return read;
}