//
// Created by gaspar on 22/10/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_BSP_H
#define FSM_PUMP_CONTROLLER_V1_BSP_H


#include <stdint-gcc.h>
#include <time.h>

#define BTN_TYPE_START 0
#define BTN_TYPE_STOP BTN_TYPE_START + 1
#define BTN_TYPE_AUTO BTN_TYPE_STOP + 1
#define BTN_TYPE_TOTAL BTN_TYPE_AUTO + 1

#define EVT_BTN_PUSH 0

void LCD_writeLine1(const uint8_t * str);
void LCD_writeLine2(const uint8_t * str);

void SYSTEM_init();
void SYSTEM_set_tick_callback(void (*func) (void));
void SYSTEM_set_button_callback(void (*func) (uint8_t button, uint8_t evt));
void SYSTEM_set_tank_millivolts_callback(void (*func) (uint32_t mVolts));
void SYSTEM_get_batt_level();
void SYSTEM_get_tank_level();
uint32_t SYSTEM_getEpoch();
struct tm SYSTEM_getTimeDate();
void SYSTEM_set_led_auto_on();
void SYSTEM_set_led_auto_off();



// END: System

#endif //FSM_PUMP_CONTROLLER_V1_BSP_H
