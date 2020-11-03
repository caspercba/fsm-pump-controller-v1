#include <stdio.h>
#include "bsp.h"
#include "fsm.h"

void callback_button(uint8_t btn, uint8_t evt);
void tick_callback(void);
void tank_callback(uint32_t mVolts);

void main(void) {

    SYSTEM_init();
    SYSTEM_set_button_callback(callback_button);
    SYSTEM_set_tick_callback(tick_callback);
    SYSTEM_set_tank_millivolts_callback(tank_callback);

    sm_handle_event(pumpfsm, CALENDAR_START);

    while (1) {

    }
}

void callback_button(uint8_t btn, uint8_t evt) {
    //sm_handle_event(pumpfsm, CALENDAR_START);
}

void tick_callback(void) {
    struct tm time = SYSTEM_getTimeDate();
    uint8_t str[12];
    sprintf(str, "%02d:%02d:%02d", time.tm_hour, time.tm_min, time.tm_sec);
    LCD_writeLine1(str);
    sprintf(str, "%02d:%02d:%04d", time.tm_mday, time.tm_mon, time.tm_year);
    LCD_writeLine2(str);
}

void tank_callback(uint32_t mVolts) {

}


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
