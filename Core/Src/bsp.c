//
// Created by gaspar on 22/10/20.
//


#include <memory.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"
#include "bsp.h"
#include "lcdhitachi.h"
#include "main.h"


static void SystemClock_Config(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
void Error_Handler() {}

static time_t getTimeStamp(RTC_TimeTypeDef*, RTC_DateTypeDef*, struct tm *);

I2C_HandleTypeDef hi2c1;
ADC_HandleTypeDef hadc1;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;
TIM_HandleTypeDef tim4;

RTC_TimeTypeDef currentTime;
RTC_DateTypeDef currentDate;


#define TICK_REFRESH_RATE_PER_SEC 4


typedef struct {
    uint16_t pin;
    void *gpioTypeDef;
} str_gpio;

typedef struct {
    str_gpio buttons[BTN_TYPE_TOTAL];
    str_gpio pump;
    str_gpio led_error;
    str_gpio led_activity;
    str_gpio led_pump_run;
    void (*callback_button) (uint8_t btn, uint8_t evt);
    void (*callback_tank_volts) (void (*func) (uint32_t mVolts));
    void (*callback_tick) (void);
} str_system;

str_system system = {
        {{GPIO_PIN_0, GPIOA},   // BTN_TYPE_START
         {GPIO_PIN_3, GPIOA},   // BTN_TYPE_STOP
         {GPIO_PIN_3, GPIOA}},  // BTN_TYPE_AUTO
        {GPIO_PIN_2, GPIOA},
        {GPIO_PIN_2, GPIOA},
        {GPIO_PIN_2, GPIOA},
        {GPIO_PIN_2, GPIOA},
        NULL,
        NULL,
        NULL
};

void LCD_writeLine1(const uint8_t * str) {
    LCD_1stLine();
    LCD_printf(str);
}
void LCD_writeLine2(const uint8_t* str) {
    LCD_2ndLine();
    LCD_printf(str);
}

void SYSTEM_init(void) {
    HAL_Init();
    SystemClock_Config();
    MX_TIM4_Init();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_ADC1_Init();
    MX_RTC_Init();
    MX_SPI1_Init();
    MX_USART2_UART_Init();
    LCD_init(&hi2c1);
}

void SYSTEM_set_button_callback(void (*func) (uint8_t button, uint8_t evt)) {
    system.callback_button = func;
}

void SYSTEM_set_tick_callback(void (*func) (void)) {
    system.callback_tick = func;
}

void SYSTEM_set_tank_millivolts_callback(void (*func) (uint32_t mVolts)) {
    system.callback_tank_volts = func;
}

uint32_t SYSTEM_getEpoch() {

}

struct tm SYSTEM_getTimeDate() {
    struct tm result;

    HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);

    result.tm_sec = currentTime.Seconds;
    result.tm_min = currentTime.Minutes;
    result.tm_hour = currentTime.Hours;
    result.tm_mday = currentDate.Date;
    result.tm_mon = currentDate.Month;
    result.tm_year = currentDate.Year + 2000;
    return result;
}

static time_t getTimeEpoch(struct tm *currentTime) {
    return mktime(currentTime);
}

static void getTimeDate(RTC_TimeTypeDef* currentTimeIn, RTC_DateTypeDef* currentDateIn, struct tm *currTimeOut) {
    uint8_t result1 = HAL_RTC_GetTime(&hrtc, currentTimeIn, RTC_FORMAT_BIN);
    uint8_t result2 = HAL_RTC_GetDate(&hrtc, currentDateIn, RTC_FORMAT_BIN);
    if ((result1 && result2) == HAL_OK) {
    currTimeOut->tm_year = currentDateIn->Year + 100;  // In fact: 2000 + 18 - 1900
    currTimeOut->tm_mday = currentDateIn->Date;
    currTimeOut->tm_mon = currentDateIn->Month - 1;
    currTimeOut->tm_hour = currentTimeIn->Hours;
    currTimeOut->tm_min = currentTimeIn->Minutes;
    currTimeOut->tm_sec = currentTimeIn->Seconds;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    for(int i = 0 ; i < BTN_TYPE_TOTAL ; i++) {
        if(system.buttons->pin == GPIO_Pin && system.callback_button != NULL) {
            system.callback_button(i, EVT_BTN_PUSH);
        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc1_ref) {
    if(system.callback_tank_volts != NULL) {
        uint32_t value = HAL_ADC_GetValue(hadc1_ref);
        system.callback_tank_volts((uint16_t) value);
    }
}

/**
 *  Configuration stuff
 */
static void SystemClock_Config(void)
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
    RCC_OscInitStruct.PLL.PLLN = 50;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

static void MX_RTC_Init(void)
{
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

}

static void MX_I2C1_Init(void)
{
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
}

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = system.buttons[BTN_TYPE_START].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(system.buttons[BTN_TYPE_START].gpioTypeDef, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0); // <--- This and
    HAL_NVIC_EnableIRQ(EXTI0_IRQn); // <--- this are what were missing for you.
}



static void MX_SPI1_Init(void)
{
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
}

static void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_TIM4_Init(void) {
    __TIM3_CLK_ENABLE();
    tim4.Init.Prescaler = 12500;
    tim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim4.Init.Period = 1000 / TICK_REFRESH_RATE_PER_SEC;
    tim4.Instance = TIM3;   //Same timer whose clocks we enabled
    HAL_TIM_Base_Init(&tim4);     // Init timer
    HAL_TIM_Base_Start_IT(&tim4); // start timer interrupts
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void) {
    //In case other interrupts are also running
    if (__HAL_TIM_GET_FLAG(&tim4, TIM_FLAG_UPDATE) != RESET) {
        if (__HAL_TIM_GET_ITSTATUS(&tim4, TIM_IT_UPDATE) != RESET) {
            __HAL_TIM_CLEAR_FLAG(&tim4, TIM_FLAG_UPDATE);
            if (system.callback_tick != NULL) {
                system.callback_tick();
            }
        }
    }
}



