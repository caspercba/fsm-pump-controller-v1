//
// Created by gaspar on 24/11/20.
//

#include <stdint-gcc.h>
#include <queue.h>
#include "menu.h"
#include "lcd_helper.h"

static uint8_t cur_state = ST_CFG_START_HOUR;
static str_fsm_configuration *config;
static uint8_t blink = 0;

static uint8_t update_value(uint16_t* val, uint8_t max, uint8_t min, uint8_t up);
static void draw();

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
    switch (cur_state) {
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

static uint8_t on_start_hour(uint32_t btn) {
    return update_value(&config->start_hour, 23, 0, btn == BTN_UP);
}
static uint8_t on_start_min(uint32_t btn) {
    return update_value(&config->start_min, 59, 0, btn == BTN_UP);
}
static uint8_t on_stop_hour(uint32_t btn) {
    return update_value(&config->stop_hour, 23, 0, btn == BTN_UP);
}
static uint8_t on_stop_min(uint32_t btn) {
    return update_value(&config->stop_min, 59, 0, btn == BTN_UP);
}
static uint8_t on_max_liters(uint32_t btn) {
    return update_value(&config->tank_capacity_liters, 10000, 100, btn == BTN_UP);
}
static uint8_t on_min_liters(uint32_t btn) {
    return update_value(&config->tank_min_liters, 10000, 100, btn == BTN_UP);
}
static uint8_t on_min_pump_rate(uint32_t btn) {
    return update_value(&config->tank_min_liters, 100, 1, btn == BTN_UP);
}
static uint8_t on_liters_per_step(uint32_t btn) {
    return update_value(&config->tank_min_liters, 100, 1, btn == BTN_UP);
}
static uint8_t on_millis_per_step(uint32_t btn) {
    return update_value(&config->tank_min_liters, 1000, 1, btn);
}

static uint8_t update_value(uint16_t* val, uint8_t max, uint8_t min, uint8_t btn) {
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
            break;
        case BTN_BACK:
            cur_state--;
            if(cur_state < 0) {
                cur_state = 0;
                return FALSE;
            }
            break;
    }

}