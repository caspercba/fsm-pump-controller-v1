//
// Created by gaspar on 24/11/20.
//

#include <stdint-gcc.h>
#include <queue.h>
#include "menu.h"
#include "lcd_helper.h"

static uint8_t cur_state = ST_CFG_INIT;
static str_fsm_configuration *config;
static uint8_t blink = 0;

// TIMEDATE STUFF
static uint16_t day = 0;
static uint16_t month = 0;
static uint16_t year = 0;
static uint16_t hour = 0;
static uint16_t min = 0;
static uint16_t sec = 0;

static uint8_t update_value(uint16_t* val, uint16_t max, uint16_t min, uint8_t up);
static void draw();


static uint8_t on_init(uint32_t btn);
static uint8_t on_set_day(uint32_t btn);
static uint8_t on_set_month(uint32_t btn);
static uint8_t on_set_year(uint32_t btn);
static uint8_t on_set_hour(uint32_t btn);
static uint8_t on_set_min(uint32_t btn);
static uint8_t on_set_sec(uint32_t btn);
static uint8_t on_start_hour(uint32_t btn);
static uint8_t on_start_min(uint32_t btn);
static uint8_t on_stop_hour(uint32_t btn);
static uint8_t on_stop_min(uint32_t btn);
static uint8_t on_max_liters(uint32_t btn);
static uint8_t on_min_liters(uint32_t btn);
static uint8_t on_min_pump_rate(uint32_t btn);
static uint8_t on_liters_per_step(uint32_t btn);
static uint8_t on_millis_per_step(uint32_t btn);




static uint8_t (*handler[ST_CFG_NUM_ST])(uint32_t btn) = {
        // ST_CFG_INIT
        on_init,
        // ST_CFG_SET_DAY
        on_set_day,
        // ST_CFG_SET_MONTH
        on_set_month,
        // ST_CFG_SET_YEAR
        on_set_year,
        // ST_CFG_SET_HOUR
        on_set_hour,
        // ST_CFG_SET_MIN
        on_set_min,
        // ST_CFG_SET_SEC
        on_set_sec,
        // ST_CFG_START_HOUR
        on_start_hour,
        // ST_CFG_START_MIN
        on_start_min,
        // ST_CFG_STOP_HOUR
        on_stop_hour,
        // ST_CFG_STOP_MIN
        on_stop_min,
        // ST_CFG_MAX_LITERS
        on_max_liters,
        // ST_CFG_MIN_LITERS
        on_min_liters,
        // ST_CFG_MIN_PUMP_RATE
        on_min_pump_rate
};

static void draw() {
    struct tm * td;
    LCD_clear();

    switch (cur_state) {
        case ST_CFG_INIT:
            td = SYSTEM_getTimeDate();
            hour = td->tm_hour;
            min = td->tm_min;
            sec = td->tm_sec;
            year = td->tm_year;
            month = td->tm_mon;
            day = td->tm_mday;
            cur_state++;
            break;
        case ST_CFG_SET_DAY:
            LCD_setCursor(0, 0);
            LCD_printf("Set date");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", day);
                LCD_printf("/");
                LCD_printf("%02d", month);
                LCD_printf("/");
                LCD_printf("%04d", year);
            } else {
                LCD_setCursor(1,2);
                LCD_printf("/");
                LCD_printf("%02d", month);
                LCD_printf("/");
                LCD_printf("%04d", year);
            }
            break;
        case ST_CFG_SET_MONTH:
            LCD_setCursor(0, 0);
            LCD_printf("Set date");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", day);
                LCD_printf("/");
                LCD_printf("%02d", month);
                LCD_printf("/");
                LCD_printf("%04d", year);
            } else {
                LCD_setCursor(1,0);
                LCD_printf("%02d", day);
                LCD_printf("/");
                LCD_setCursor(1,5);
                LCD_printf("/");
                LCD_printf("%04d", year);
            }
            break;
        case ST_CFG_SET_YEAR:
            LCD_setCursor(0, 0);
            LCD_printf("Set date");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", day);
                LCD_printf("/");
                LCD_printf("%02d", month);
                LCD_printf("/");
                LCD_printf("%04d", year);
            } else {
                LCD_setCursor(1,0);
                LCD_printf("%02d", day);
                LCD_printf("/");
                LCD_printf("%02d", month);
                LCD_printf("/");
            }
            break;
        case ST_CFG_SET_HOUR:
            LCD_setCursor(0, 0);
            LCD_printf("Set Time");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", hour);
                LCD_printf(":");
                LCD_printf("%02d", min);
                LCD_printf(":");
                LCD_printf("%02d", sec);
            } else {
                LCD_setCursor(1,2);
                LCD_printf(":");
                LCD_printf("%02d", min);
                LCD_printf(":");
                LCD_printf("%02d", sec);
            }
            break;
        case ST_CFG_SET_MIN:
            LCD_setCursor(0, 0);
            LCD_printf("Set Time");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", hour);
                LCD_printf(":");
                LCD_printf("%02d", min);
                LCD_printf(":");
                LCD_printf("%02d", sec);
            } else {
                LCD_setCursor(1,0);
                LCD_printf("%02d", hour);
                LCD_printf(":");
                LCD_setCursor(1,5);
                LCD_printf(":");
                LCD_printf("%02d", sec);
            }
            break;
        case ST_CFG_SET_SEC:
            LCD_setCursor(0, 0);
            LCD_printf("Set Time");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", hour);
                LCD_printf(":");
                LCD_printf("%02d", min);
                LCD_printf(":");
                LCD_printf("%02d", sec);
            } else {
                LCD_setCursor(1,0);
                LCD_printf("%02d", hour);
                LCD_printf(":");
                LCD_printf("%02d", min);
                LCD_printf(":");
            }
            break;
        case ST_CFG_START_HOUR:
            LCD_setCursor(0, 0);
            LCD_printf("Pump start time");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", config->start_hour);
                LCD_printf(":");
                LCD_printf("%02d", config->start_min);
            } else {
                LCD_setCursor(1,2);
                LCD_printf(":");
                LCD_printf("%02d", config->start_min);
            }
            break;
        case ST_CFG_START_MIN:
            LCD_setCursor(0, 0);
            LCD_printf("Pump start time");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", config->start_hour);
                LCD_printf(":");
                LCD_printf("%02d", config->start_min);
            } else {
                LCD_setCursor(1,0);
                LCD_printf("%02d", config->start_hour);
                LCD_printf(":");
            }
            break;
        case ST_CFG_STOP_HOUR:
            LCD_setCursor(0, 0);
            LCD_printf("Pump stop time");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", config->stop_hour);
                LCD_printf(":");
                LCD_printf("%02d", config->stop_min);
            } else {
                LCD_setCursor(1,2);
                LCD_printf(":");
                LCD_printf("%02d", config->stop_min);
            }
            break;
        case ST_CFG_STOP_MIN:
            LCD_setCursor(0, 0);
            LCD_printf("Pump stop time");
            LCD_setCursor(1,0);
            if(blink % 2 == 0) {
                LCD_printf("%02d", config->stop_hour);
                LCD_printf(":");
                LCD_printf("%02d", config->stop_min);
            } else {
                LCD_setCursor(1,0);
                LCD_printf("%02d", config->stop_hour);
                LCD_printf(":");
            }
            break;
        case ST_CFG_MAX_LITERS:
            LCD_setCursor(0, 0);
            LCD_printf("Tank capacity:");
            if(blink % 2 == 0) {
                LCD_setCursor(1, 8);
                LCD_printf("%06dL", config->tank_capacity_liters);
            }
            break;
        case ST_CFG_MIN_LITERS:
            LCD_setCursor(0, 0);
            LCD_printf("Tank min liters:");
            if(blink % 2 == 0) {
                LCD_setCursor(1, 8);
                LCD_printf("%06dL", config->tank_min_liters);
            }
            break;
        case ST_CFG_MIN_PUMP_RATE:
            LCD_setCursor(0, 0);
            LCD_printf("Pump rate (l/m)");
            if(blink % 2 == 0) {
                LCD_setCursor(1, 5);
                LCD_printf("%03d(l/m)", config->pump_min_rate_liters_per_min);
            }
            break;


    }
}

void MENU_init(str_fsm_configuration* cfg) {
    config = cfg;
}

uint8_t MENU_handle_button(uint32_t button) {
    return handler[cur_state](button);
}

void MENU_tick() {
    draw();
    blink++;
}




static uint8_t on_init(uint32_t btn) {
    // do nothing
}
static uint8_t on_set_day(uint32_t btn) {
    return update_value(&day, 31, 1, btn);
}
static uint8_t on_set_month(uint32_t btn) {
    return update_value(&month, 12, 1, btn);
}
static uint8_t on_set_year(uint32_t btn) {
    uint8_t result = update_value(&year, 2030, 2020, btn);
    if(result == TRUE) SYSTEM_setDate(day, month, year - 2000);
    return result;
}
static uint8_t on_set_hour(uint32_t btn) {
    return update_value(&hour, 23, 0, btn);
}
static uint8_t on_set_min(uint32_t btn) {
    return update_value(&min, 59, 0, btn);
}
static uint8_t on_set_sec(uint32_t btn) {
    uint8_t result = update_value(&sec, 59, 0, btn);
    if(result == TRUE) SYSTEM_setTime(hour, min, sec);
    return result;
}

static uint8_t on_start_hour(uint32_t btn) {
    return update_value(&config->start_hour, 23, 0, btn);
}
static uint8_t on_start_min(uint32_t btn) {
    return update_value(&config->start_min, 59, 0, btn);
}
static uint8_t on_stop_hour(uint32_t btn) {
    return update_value(&config->stop_hour, 23, 0, btn);
}
static uint8_t on_stop_min(uint32_t btn) {
    return update_value(&config->stop_min, 59, 0, btn);
}
static uint8_t on_max_liters(uint32_t btn) {
    return update_value(&config->tank_capacity_liters, 10000, 100, btn);
}
static uint8_t on_min_liters(uint32_t btn) {
    return update_value(&config->tank_min_liters, 10000, 100, btn);
}
static uint8_t on_min_pump_rate(uint32_t btn) {
    return update_value(&config->tank_min_liters, 100, 1, btn);
}
static uint8_t on_liters_per_step(uint32_t btn) {
    return update_value(&config->tank_min_liters, 100, 1, btn);
}
static uint8_t on_millis_per_step(uint32_t btn) {
    return update_value(&config->tank_min_liters, 1000, 1, btn);
}

static uint8_t update_value(uint16_t* val, uint16_t max, uint16_t min, uint8_t btn) {
    switch(btn) {
        case BTN_UP:
            if(*val < max) *val = (*val) + 1;
            return TRUE;
            break;
        case BTN_DOWN:
            if(*val > min) *val = (*val) - 1;
            return TRUE;
            break;
        case BTN_OK:
            cur_state++;
            if(cur_state >= ST_CFG_NUM_ST) {
                cur_state = 0;
                return FALSE;
            }
            return TRUE;
            break;
        case BTN_BACK:
            cur_state--;
            if(cur_state < 0) {
                cur_state = 0;
                return FALSE;
            }
            return TRUE;
            break;
    }

}