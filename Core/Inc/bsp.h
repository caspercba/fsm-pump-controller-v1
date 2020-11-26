//
// Created by gaspar on 22/10/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_BSP_H
#define FSM_PUMP_CONTROLLER_V1_BSP_H


#include <stdint-gcc.h>
#include <time.h>

#define TRUE 0
#define FALSE 1
//#define DEBUG

#define BTN_TYPE_BTN0 0
#define BTN_TYPE_BTN1 BTN_TYPE_BTN0 + 1
#define BTN_TYPE_BTN2 BTN_TYPE_BTN1 + 1
#define BTN_TYPE_BTN3 BTN_TYPE_BTN2 + 1
#define BTN_TYPE_TOTAL BTN_TYPE_BTN3 + 1

#define BTN_FORCE_START     BTN_TYPE_BTN0
#define BTN_MODE            BTN_TYPE_BTN1
#define BTN_AUTO            BTN_TYPE_BTN2
#define BTN_CONFIG            BTN_TYPE_BTN3

#define BTN_UP      BTN_TYPE_BTN1
#define BTN_DOWN    BTN_TYPE_BTN2
#define BTN_OK      BTN_TYPE_BTN0
#define BTN_BACK    BTN_TYPE_BTN3



/*
 ___ _   _ ___ _____
|_ _| \ | |_ _|_   _|
 | ||  \| || |  | |
 | || |\  || |  | |
|___|_| \_|___| |_|

 */
void SYSTEM_init();
void SYSTEM_delay(uint32_t delay);


/*
  ____    _    _     _     ____    _    ____ _  ______
 / ___|  / \  | |   | |   | __ )  / \  / ___| |/ / ___|
| |     / _ \ | |   | |   |  _ \ / _ \| |   | ' /\___ \
| |___ / ___ \| |___| |___| |_) / ___ \ |___| . \ ___) |
 \____/_/   \_\_____|_____|____/_/   \_\____|_|\_\____/

*/
void SYSTEM_set_tick_callback(void (*func) (void));
void SYSTEM_set_button_callback(void (*func) (uint8_t button));
void SYSTEM_set_tank_liters_callback(void (*func) (uint32_t mVolts));
void SYSTEM_set_batt_millivolts_callback(void (*func) (uint32_t mVolts));



/*
 _____ ___ __  __ _____
|_   _|_ _|  \/  | ____|
  | |  | || |\/| |  _|
  | |  | || |  | | |___
  |_| |___|_|  |_|_____|

 */
time_t SYSTEM_getEpoch();
struct tm *SYSTEM_getTimeDate();
void SYSTEM_setTime(uint8_t hour, uint8_t min, uint8_t sec);
void SYSTEM_setDate(uint8_t day, uint8_t month, uint16_t year);


/*
_     _____ ____  ____
| |   | ____|  _ \/ ___|
| |   |  _| | | | \___ \
| |___| |___| |_| |___) |
|_____|_____|____/|____/
*/

// 0: off  ,  1: on
void SYSTEM_led_auto_set(uint8_t);
uint8_t SYSTEM_led_auto_is_on();



/*
 ____  _   _ __  __ ____
|  _ \| | | |  \/  |  _ \
| |_) | | | | |\/| | |_) |
|  __/| |_| | |  | |  __/
|_|    \___/|_|  |_|_|

 */
 uint8_t SYSTEM_pump_is_on();
void SYSTEM_pump_set(uint8_t val);





#endif //FSM_PUMP_CONTROLLER_V1_BSP_H
