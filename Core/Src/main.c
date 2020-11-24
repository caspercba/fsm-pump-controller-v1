#include "bsp.h"
#include "fsm.h"

void callback_button(uint8_t btn);
void tick_callback(void);
void tank_callback(uint32_t liters);
void batt_callback(uint32_t mVolts);

str_fsm_configuration my_config = {
        8000,
        5000,
        25,
        48000,
        10,
        00,
        16,
        00,
        TRUE
};

int main(void) {

    SYSTEM_init();
    SYSTEM_set_button_callback(callback_button);
    SYSTEM_set_tick_callback(tick_callback);
    SYSTEM_set_tank_liters_callback(tank_callback);
    SYSTEM_set_batt_millivolts_callback(batt_callback);

    fsm_set_config(&my_config);

    while (1) {
        fsm_handle_events();
        SYSTEM_delay(100);
    }
}

void callback_button(uint8_t btn) {
    str_event ev = { EV_BUTTON_PUSH, btn, SYSTEM_getEpoch()};
    fsm_enqueue_event(ev);
}

void tick_callback(void) {
    uint32_t epoch = SYSTEM_getEpoch();
    str_event ev = { EV_TIMER_TICK, SYSTEM_getTimeDate() , epoch};
    fsm_enqueue_event(ev);
}

// cached values
uint32_t last_tank_level = 10000;
uint32_t last_tank_level_time = 0;
uint8_t tank_counter = 0;

void tank_callback(uint32_t liters) {
    str_event ev = { EV_TANK_LEVEL, liters, SYSTEM_getEpoch()};
    fsm_enqueue_event(ev);
    tank_counter++;

    // Send pump rate event, estimating values
    if(tank_counter >= 10) {
        tank_counter=0;
        uint32_t now = SYSTEM_getEpoch();
        uint32_t pumprate = ((liters - last_tank_level) * 60) / (SYSTEM_getEpoch() - last_tank_level_time);
        last_tank_level_time = now;
        last_tank_level = liters;
        ev.id = EV_PUMP_RATE; ev.data = pumprate; ev.epoch = SYSTEM_getEpoch();
        fsm_enqueue_event(ev);
    }
}

void batt_callback(uint32_t mVolts) {
    str_event ev = { EV_BATT_LEVEL, mVolts, SYSTEM_getEpoch()};
    fsm_enqueue_event(ev);
}