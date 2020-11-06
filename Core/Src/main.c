#include <stdio.h>
#include "bsp.h"
#include "fsm.h"
#include "stm32f4xx_hal.h"

void callback_button(uint8_t btn, uint8_t evt);
void tick_callback(void);
void tank_callback(uint32_t mVolts);


uint32_t delay = 100;

void main(void) {

    SYSTEM_init();
    SYSTEM_set_button_callback(callback_button);
    SYSTEM_set_tick_callback(tick_callback);
    SYSTEM_set_tank_millivolts_callback(tank_callback);


    while (1) {
        printf("ENQUEUE HANDLE\r\n");
        fsm_handle_events();
        HAL_Delay(delay);
        delay += 10;
        if(delay > 600) delay = 100;
    }
}

void callback_button(uint8_t btn, uint8_t evt) {
    //sm_handle_event(pumpfsm, EV_BUTTON_PUSH, &btn);
//    uint8_t str[12];
//    sprintf(str, "BUTTON PRESS: %d", btn);
//    LCD_writeLine1(str);
}

uint8_t count = 0;

void tick_callback(void) {
    if(count % 4) {
        printf("ENQUEUE: EV_BUTTON_PUSH, data: data\r\n");
        str_event ev = { EV_BUTTON_PUSH, 111};
        fsm_enqueue_event(ev);
    } else if(count % 3) {
        printf("ENQUEUE: EV_TANK_LEVEL, data: data\r\n");
        str_event ev = { EV_TANK_LEVEL, 222};
        fsm_enqueue_event(ev);
    } else if (count % 2) {
        printf("ENQUEUE: EV_TIMER_TICK, data: data\r\n");
        str_event ev = { EV_TIMER_TICK, 333};
        fsm_enqueue_event(ev);
    }
    else {
        printf("ENQUEUE: EV_CALENDAR, data: 123\r\n");
        str_event ev = { EV_CALENDAR, 444};
        fsm_enqueue_event(ev);
    }
//    struct tm time = SYSTEM_getTimeDate();
//    uint8_t str[12];
//    sprintf(str, "%02d:%02d:%02d", time.tm_hour, time.tm_min, time.tm_sec);
//    LCD_writeLine1(str);
//    sprintf(str, "%02d:%02d:%04d", time.tm_mday, time.tm_mon, time.tm_year);
//    LCD_writeLine2(str);
count++;
}

void tank_callback(uint32_t mVolts) {
    //sm_handle_event(pumpfsm, EV_TANK_LEVEL, &mVolts);
}


/* USER CODE END PFP */



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
