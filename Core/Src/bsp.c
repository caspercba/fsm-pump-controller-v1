//
// Created by gaspar on 22/10/20.
//

#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"
#include "bsp.h"
#include "lcdhitachi.h"
#include "main.h"
#include "eeprom.h"


// DEBUG STUFF

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */




void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC2_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM6_Init(void);
void Error_Handler() {}

static time_t getTimeStamp(RTC_TimeTypeDef*, RTC_DateTypeDef*, struct tm *);


ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
I2C_HandleTypeDef hi2c1;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim6;
UART_HandleTypeDef huart2;

RTC_TimeTypeDef currentTime;
RTC_DateTypeDef currentDate;
struct tm tm_last_result;
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x0, 0x6666, 0x7777};


typedef struct {
    uint16_t pin;
    GPIO_TypeDef *gpioTypeDef;
} str_gpio;

typedef struct {
    str_gpio buttons[BTN_TYPE_TOTAL];
    str_gpio pump;
    str_gpio led_error;
    str_gpio led_activity;
    str_gpio led_pump_run;
    str_gpio led_auto_on;
    void (*callback_button) (uint8_t btn);
    void (*callback_tank_liters) (uint32_t);
    void (*callback_batt_volts) (uint32_t);
    void (*callback_tick) (void);
    uint32_t tank_liters_per_step;
    uint32_t tank_step_offset;
    double millivolt_per_step_batt;
    uint8_t tick_refresh_times_per_sec;
    uint8_t tank_refresh_every_secs;
} str_hw_config;

str_hw_config hw_config = {
        {{GPIO_PIN_3, GPIOB},   // BTN_TYPE_BTN0
         {GPIO_PIN_4, GPIOB},   // BTN_TYPE_BTN1
         {GPIO_PIN_5, GPIOB},   // BTN_TYPE_BTN2
         {GPIO_PIN_7, GPIOD}},  // BTN_TYPE_BTN3
        {GPIO_PIN_6, GPIOD},
        {LD5_Pin, GPIOD},
        {LD4_Pin, GPIOD},
        {LD3_Pin, GPIOD},
        {GPIO_PIN_6, GPIOD},
        NULL,
        NULL,
        NULL,
        NULL,
        2,
        0,
        0.025,
        5,
        1
};

static uint8_t isPinOn(str_gpio* gpio) {
    if(HAL_GPIO_ReadPin(gpio->gpioTypeDef, gpio->pin) == GPIO_PIN_SET) return 1;
    else return 0;
}

static void setPin(str_gpio *gpio, uint8_t value) {
    if(value != 0) HAL_GPIO_WritePin(gpio->gpioTypeDef, gpio->pin, GPIO_PIN_SET);
    else HAL_GPIO_WritePin(gpio->gpioTypeDef, gpio->pin, GPIO_PIN_RESET);
}

void SYSTEM_init(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
//#ifdef DBG
    MX_USART2_UART_Init();
//#else
    MX_I2C1_Init();
    LCD_init(&hi2c1);
//#endif


    MX_ADC2_Init();

    MX_RTC_Init();
    //MX_SPI1_Init();
    MX_TIM6_Init();

    HAL_ADC_Start_IT(&hadc1);
    HAL_ADC_Start_IT(&hadc2);
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_FLASH_Unlock();

    if( EE_Init() != EE_OK)
    {
        Error_Handler();
    }
}

void SYSTEM_delay(uint32_t delay) {
    HAL_Delay(delay);
}

void SYSTEM_set_button_callback(void (*func) (uint8_t button)) {
    hw_config.callback_button = func;
}

void SYSTEM_set_tick_callback(void (*func) (void)) {
    hw_config.callback_tick = func;
}

void SYSTEM_set_tank_liters_callback(void (*func) (uint32_t mVolts)) {
    hw_config.callback_tank_liters = func;
}

void SYSTEM_set_batt_millivolts_callback(void (*func) (uint32_t mVolts)) {
    hw_config.callback_batt_volts = func;
}

static time_t getTimeEpoch() {
    struct tm *temp = SYSTEM_getTimeDate();
    return mktime(temp);
}

time_t SYSTEM_getEpoch() {
    return getTimeEpoch();
}

//struct tm * SYSTEM_getDifference(uint32_t past, uint32_t future) {
//    uint32_t seconds = difftime(mktime(future), mktime(past));
//    return
//}

void SYSTEM_led_auto_set(uint8_t value) {
    setPin(&hw_config.led_auto_on, value);
}

uint8_t SYSTEM_led_auto_is_on() {
    return isPinOn(&hw_config.led_auto_on);
}

uint8_t SYSTEM_pump_is_on() {
    return isPinOn(&hw_config.pump.pin);
}

void SYSTEM_pump_set(uint8_t val) {
    setPin(&hw_config.pump, val);
}

struct tm *SYSTEM_getTimeDate() {


    HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);

    tm_last_result.tm_sec = currentTime.Seconds;
    tm_last_result.tm_min = currentTime.Minutes;
    tm_last_result.tm_hour = currentTime.Hours;
    tm_last_result.tm_mday = currentDate.Date;
    tm_last_result.tm_mon = currentDate.Month;
    tm_last_result.tm_year = currentDate.Year + 2000;
    return &tm_last_result;
}

void SYSTEM_setTime(uint8_t hour, uint8_t min, uint8_t sec) {
    RTC_TimeTypeDef time;
    time.Hours = hour;
    time.Minutes = min;
    time.Seconds = sec;
    HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
}
void SYSTEM_setDate(uint8_t day, uint8_t month, uint16_t year) {
    RTC_DateTypeDef date;
    date.Year = year;
    date.Month = month;
    date.Date = day;
    HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
}

void SYSTEM_save(str_fsm_configuration* data){

    uint16_t c_addr = 0;
    EE_WriteVariable(c_addr, data->tank_capacity_liters);
    c_addr+=16;
    EE_WriteVariable(c_addr, data->tank_min_liters);
    c_addr+=16;
    EE_WriteVariable(c_addr, data->pump_min_rate_liters_per_min);
    c_addr+=16;
    EE_WriteVariable(c_addr, (uint32_t) (data->batt_min_millivolts & 0xFFFF));
    c_addr+=16;
    EE_WriteVariable(c_addr, (uint32_t) (data->batt_min_millivolts>>16));
    c_addr+=16;
    EE_WriteVariable(c_addr, data->start_hour);
    c_addr+=16;
    EE_WriteVariable(c_addr, data->start_min);
    c_addr+=16;
    EE_WriteVariable(c_addr, data->stop_hour);
    c_addr+=16;
    EE_WriteVariable(c_addr, data->stop_min);
    c_addr+=16;
    EE_WriteVariable(c_addr, data->auto_enabled);
    c_addr+=16;
    EE_WriteVariable(c_addr, data->startup_wait_secs);

}
void SYSTEM_read(str_fsm_configuration* data) {
    uint16_t c_addr = 0;
    EE_ReadVariable(c_addr, &data->tank_capacity_liters);
    c_addr+=16;
    EE_ReadVariable(c_addr, &data->tank_min_liters);
    c_addr+=16;
    EE_ReadVariable(c_addr, &data->pump_min_rate_liters_per_min);
    c_addr+=16;
    uint16_t volts = 0;
    EE_ReadVariable(c_addr, &volts);
    c_addr+=16;
    data->batt_min_millivolts = volts;
    EE_ReadVariable(c_addr, &volts);
    c_addr+=16;
    data->batt_min_millivolts = data->batt_min_millivolts | volts << 16;
    EE_ReadVariable(c_addr, &data->start_hour);
    c_addr+=16;
    EE_ReadVariable(c_addr, &data->start_min);
    c_addr+=16;
    EE_ReadVariable(c_addr, &data->stop_hour);
    c_addr+=16;
    EE_ReadVariable(c_addr, &data->stop_min);
    c_addr+=16;
    EE_ReadVariable(c_addr, &data->auto_enabled);
    c_addr+=16;
    EE_ReadVariable(c_addr, &data->startup_wait_secs);
}



/**
 *  Configuration stuff
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */
    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

    /* USER CODE BEGIN ADC2_Init 0 */

    /* USER CODE END ADC2_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC2_Init 1 */

    /* USER CODE END ADC2_Init 1 */
    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc2.Instance = ADC2;
    hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
    hadc2.Init.Resolution = ADC_RESOLUTION_12B;
    hadc2.Init.ScanConvMode = DISABLE;
    hadc2.Init.ContinuousConvMode = ENABLE;
    hadc2.Init.DiscontinuousConvMode = DISABLE;
    hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc2.Init.NbrOfConversion = 1;
    hadc2.Init.DMAContinuousRequests = DISABLE;
    hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc2) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC2_Init 2 */

    /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

    /* USER CODE BEGIN RTC_Init 0 */

    /* USER CODE END RTC_Init 0 */

    /* USER CODE BEGIN RTC_Init 1 */

    /* USER CODE END RTC_Init 1 */
    /** Initialize RTC Only
    */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN RTC_Init 2 */

    /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */

}


static void MX_TIM6_Init(void)
{

    /* USER CODE BEGIN TIM6_Init 0 */

    /* USER CODE END TIM6_Init 0 */

    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM6_Init 1 */

    /* USER CODE END TIM6_Init 1 */
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 8400;
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = 1000;
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM6_Init 2 */
    /* USER CODE END TIM6_Init 2 */

}


/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */

}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();


    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                             |GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);


    // configure buttons
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    for(int i=0; i < BTN_TYPE_TOTAL ; i++) {
        GPIO_InitStruct.Pin = hw_config.buttons[i].pin;
        HAL_GPIO_Init(hw_config.buttons[i].gpioTypeDef, &GPIO_InitStruct);
    }


    // boot1 pin
    GPIO_InitStruct.Pin = BOOT1_Pin;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // outputs
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // led pump
    GPIO_InitStruct.Pin = hw_config.pump.pin;
    HAL_GPIO_Init(hw_config.pump.gpioTypeDef, &GPIO_InitStruct);

    // led run
    GPIO_InitStruct.Pin = hw_config.led_pump_run.pin;
    HAL_GPIO_Init(hw_config.led_pump_run.gpioTypeDef, &GPIO_InitStruct);

    // led auto
    GPIO_InitStruct.Pin = hw_config.led_auto_on.pin;
    HAL_GPIO_Init(hw_config.led_auto_on.gpioTypeDef, &GPIO_InitStruct);

    // led activity
    GPIO_InitStruct.Pin = hw_config.led_activity.pin;
    HAL_GPIO_Init(hw_config.led_activity.gpioTypeDef, &GPIO_InitStruct);

    // led error
    GPIO_InitStruct.Pin = hw_config.led_error.pin;
    HAL_GPIO_Init(hw_config.led_error.gpioTypeDef, &GPIO_InitStruct);

    // onboard leds
    GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

uint8_t last_button = -1;
time_t last_button_time = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    for(int i = 0 ; i < BTN_TYPE_TOTAL ; i++) {
        if(hw_config.buttons[i].pin == GPIO_Pin && hw_config.callback_button != NULL) {
            if(last_button == hw_config.buttons[i].pin &&
                    (SYSTEM_getEpoch() - last_button_time) * 1000 < 5) {

            } else {
                hw_config.callback_button(i);
            }
            last_button = hw_config.buttons[i].pin;
            last_button_time = SYSTEM_getEpoch();
        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc_ref) {
    uint32_t value = HAL_ADC_GetValue(hadc_ref);
    if(hadc_ref == &hadc1) {
        if(hw_config.callback_tank_liters != NULL) {
            uint32_t tank_liters = (value - hw_config.tank_step_offset) * hw_config.tank_liters_per_step;
            hw_config.callback_tank_liters(tank_liters);
        }
    } else if(hadc_ref == &hadc2 ) {
        if(hw_config.callback_batt_volts != NULL) {
            hw_config.callback_batt_volts(value * hw_config.millivolt_per_step_batt);
        }
    }
}

uint8_t counter = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim == &htim6 && hw_config.callback_tick != NULL) {
        hw_config.callback_tick();
        if(counter % 10 == 0) {
            HAL_ADC_Start_IT(&hadc1);
            HAL_ADC_Start_IT(&hadc2);
        }
    }
    counter++;
}




PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

//int _write (int fd, char *ptr, int len)
//{
//    HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, 0xFFFF);
//
//    return len;
//}
//
//int _read (int fd, char *ptr, int len)
//{
//
//    *ptr = 0x00; // Flush the character buffer
//
//    HAL_UART_Receive(&huart2, (uint8_t*) ptr, 1, 0xFFFF);
//
//    return 1;
//}


