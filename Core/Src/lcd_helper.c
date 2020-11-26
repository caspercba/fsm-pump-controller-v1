//
// Created by gaspar on 15/11/20.
//

#include "lcd_helper.h"


uint8_t mode = MODE0;

void switch_mode(void) {
    mode++;
    if(mode >= NMODES) mode = 0;
}




/*
 ___ ____  _     _____
|_ _|  _ \| |   | ____|
 | || | | | |   |  _|
 | || |_| | |___| |___
|___|____/|_____|_____|
*/

void LCDH_idle(struct tm * time_now, uint32_t tank, uint32_t batt, uint32_t tank_percent) {
    LCD_clear();
    switch (mode) {
        case MODE0:
            LCD_setCursor(0,0);
            LCD_printf("IDLE");
            LCD_setCursor(1,6);
            LCD_printf("%02d/%02d/%02d", time_now->tm_mday, time_now->tm_mon, time_now->tm_year);
            LCD_setCursor(0,8);
            LCD_printf("%02d:%02d:%02d", time_now->tm_hour, time_now->tm_min, time_now->tm_sec);
            break;
        case MODE1:
            LCD_setCursor(0,0);
            LCD_printf("%04dL %03d%%", tank, tank_percent);
            LCD_setCursor(1,0);
            LCD_printf("%05dv", batt);
            break;

        default:
            break;
    }
}

/*
 ____ _____  __        ___    ___ _____   ____  __  __ ___ _   _
/ ___|_   _| \ \      / / \  |_ _|_   _| |___ \|  \/  |_ _| \ | |
\___ \ | |    \ \ /\ / / _ \  | |  | |     __) | |\/| || ||  \| |
 ___) || |     \ V  V / ___ \ | |  | |    / __/| |  | || || |\  |
|____/ |_|      \_/\_/_/   \_\___| |_|   |_____|_|  |_|___|_| \_|

 */

void LCDH_waiting(time_t wait_time, uint32_t pump_rate) {
    LCD_clear();
    switch(mode) {
        case MODE0:
        case MODE1:
            LCD_setCursor(0,0);
            LCD_printf("WAIT: %f:%f", wait_time, wait_time);
            LCD_setCursor(1,0);
            LCD_printf("PUMP: %03d l/min", pump_rate);
            break;
    }
}

/*
 ____  _   _ _   _ _   _ ___ _   _  ____
|  _ \| | | | \ | | \ | |_ _| \ | |/ ___|
| |_) | | | |  \| |  \| || ||  \| | |  _
|  _ <| |_| | |\  | |\  || || |\  | |_| |
|_| \_\\___/|_| \_|_| \_|___|_| \_|\____|

 */

void LCDH_running(struct tm * time_now,
        uint32_t tank,
        uint32_t batt,
        uint32_t tank_percent,
        uint32_t pump_rate) {

    LCD_clear();
    switch (mode) {
        case MODE0:
            LCD_setCursor(0,0);
            LCD_printf("RUNNING");
            LCD_setCursor(0,9);
            LCD_printf("%03d\%", tank_percent);
            LCD_setCursor(1,0);
            LCD_printf("rate(L/m) %02d", pump_rate);
            break;
        case MODE1:
            LCD_setCursor(0,0);
            LCD_printf("RUNNING");
            LCD_setCursor(0,9);
            LCD_printf("%02d%L", tank);
            LCD_setCursor(1,0);
            LCD_printf("04&d l/m");
            break;
    }
}

/*
 _____ ____  ____   ___  ____
| ____|  _ \|  _ \ / _ \|  _ \
|  _| | |_) | |_) | | | | |_) |
| |___|  _ <|  _ <| |_| |  _ <
|_____|_| \_\_| \_\\___/|_| \_\
 */
void LCDH_error(uint8_t * msg) {
    LCD_clear();
    LCD_setCursor(0,0);
    LCD_printf("ERROR");
    LCD_setCursor(1,0);
    LCD_printf(msg);
}



//void idle_show_mode1(uint8_t *time,
//               uint8_t *timeNextStart) {
//    LCD_setCursor(0,0);
//    LCD_printf("IDLE");
//    LCD_setCursor(0,8);
//    LCD_printf(time);
//    LCD_setCursor(1,0);
//    LCD_printf("Next Start:");
//    LCD_setCursor(1,8);
//    LCD_printf(timeNextStart);
//}