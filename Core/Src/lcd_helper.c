//
// Created by gaspar on 15/11/20.
//

#include "lcd_helper.h"

#define MODE0       0
#define MODE1       MODE0 + 1
//#define MODE2       MODE1 + 1
//#define NMODES      MODE2 + 1
#define NMODES      MODE1 + 1

#define MODEDURATION    20      // ticks





uint8_t state = 0;
uint8_t ticks = 0;

void show_idle(struct tm * time, uint32_t tank, uint32_t batt, uint32_t tank_percent) {
    LCD_clear();
    switch (state) {
        case MODE0:
            LCD_setCursor(0,0);
            LCD_printf("IDLE");
            LCD_setCursor(1,6);
            LCD_printf("%02d/%02d/%02d", time->tm_mday, time->tm_mon, time->tm_year);
            LCD_setCursor(0,8);
            LCD_printf("%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);
            break;
        case MODE1:
//            LCD_setCursor(0,0);
//            LCD_printf("Next Start in", 10);
//            LCD_setCursor(1,0);
//            LCD_printf("%02dh %02dm", 10, 20);
//            break;
//        case MODE2:
            LCD_setCursor(0,0);
            LCD_printf("%04dL %03d%%", tank, tank_percent);
            LCD_setCursor(1,0);
            LCD_printf("%05dv", batt);
            break;

        default:
            break;
    }

    if(ticks >= MODEDURATION) {
        state++;
        ticks = 0;
    }
    if(state >= NMODES) state = 0;

//    state = MODE2;

    ticks++;
}

void show_running(uint32_t pump_rate, uint32_t liters, uint32_t fill_percent) {
    LCD_clear();
    switch (state) {
        case 0:
            LCD_setCursor(0,0);
            LCD_printf("RUNNING");
            LCD_setCursor(0,9);
            LCD_printf("%03d\%", fill_percent);
            LCD_setCursor(1,0);
            LCD_printf("rate(L/m) %02d", pump_rate);
            break;
        case 1:
            LCD_setCursor(0,0);
            LCD_printf("RUNNING");
            LCD_setCursor(1,0);
            LCD_printf("liters %02d%", liters);
            break;
    }
    if(ticks >= MODEDURATION) {
        state++;
        ticks = 0;
    }
    if(state >= NMODES) state = 0;

    ticks++;

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