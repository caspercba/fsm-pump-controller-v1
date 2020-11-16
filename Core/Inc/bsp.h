//
// Created by gaspar on 22/10/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_BSP_H
#define FSM_PUMP_CONTROLLER_V1_BSP_H


#include <stdint-gcc.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

#define BTN_TYPE_START 0
#define BTN_TYPE_STOP BTN_TYPE_START + 1
#define BTN_TYPE_AUTO BTN_TYPE_STOP + 1
#define BTN_TYPE_TOTAL BTN_TYPE_AUTO + 1



void SYSTEM_init();

// Callbacks from interrupts
void SYSTEM_set_tick_callback(void (*func) (void));
void SYSTEM_set_button_callback(void (*func) (uint8_t button));
void SYSTEM_set_tank_liters_callback(void (*func) (uint32_t mVolts));
void SYSTEM_set_batt_millivolts_callback(void (*func) (uint32_t mVolts));


uint32_t SYSTEM_getEpoch();
struct tm *SYSTEM_getTimeDate();


// 0: off  ,  1: on
void SYSTEM_led_auto_set(uint8_t);
uint8_t SYSTEM_led_auto_is_on();

uint8_t SYSTEM_pump_is_on();
void SYSTEM_pump_set(uint8_t val);





// END: System

#endif //FSM_PUMP_CONTROLLER_V1_BSP_H
