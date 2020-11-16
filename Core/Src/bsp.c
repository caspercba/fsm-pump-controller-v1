//
// Created by gaspar on 22/10/20.
//


#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"
#include "bsp.h"
#include "lcdhitachi.h"
#include "main.h"

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


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
struct tm tm_last_result;


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
    uint32_t millivolt_per_step_batt;
    uint8_t tick_refresh_times_per_sec;
    uint8_t tank_refresh_every_secs;
} str_hw_config;

str_hw_config hw_config = {
        {{GPIO_PIN_0, GPIOE},   // BTN_TYPE_START
         {GPIO_PIN_1, GPIOE},   // BTN_TYPE_STOP
         {GPIO_PIN_2, GPIOE}},  // BTN_TYPE_AUTO
        {GPIO_PIN_3, GPIOE},
        {GPIO_PIN_4, GPIOE},
        {GPIO_PIN_5, GPIOE},
        {GPIO_PIN_6, GPIOE},
        {GPIO_PIN_0, GPIOD},
        NULL,
        NULL,
        NULL,
        NULL,
        2,
        0,
        41,
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
    MX_TIM4_Init();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_ADC1_Init();
    MX_RTC_Init();
    //MX_SPI1_Init();
    //MX_USART2_UART_Init();
    LCD_init(&hi2c1);
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

uint32_t SYSTEM_getEpoch() {
    return getTimeEpoch();
}

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
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = EOC_SINGLE_CONV;
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

}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = hw_config.buttons[BTN_TYPE_START].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(hw_config.buttons[BTN_TYPE_START].gpioTypeDef, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = hw_config.buttons[BTN_TYPE_STOP].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(hw_config.buttons[BTN_TYPE_STOP].gpioTypeDef, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = hw_config.buttons[BTN_TYPE_AUTO].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(hw_config.buttons[BTN_TYPE_AUTO].gpioTypeDef, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = hw_config.pump.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(hw_config.pump.gpioTypeDef, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = hw_config.led_pump_run.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(hw_config.led_pump_run.gpioTypeDef, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

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
    tim4.Init.Period = 1000 / hw_config.tick_refresh_times_per_sec;
    tim4.Instance = TIM3;   //Same timer whose clocks we enabled
    HAL_TIM_Base_Init(&tim4);     // Init timer
    HAL_TIM_Base_Start_IT(&tim4); // start timer interrupts
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

static uint8_t timer_count = 0;

void TIM3_IRQHandler(void) {
    //In case other interrupts are also running
    if (__HAL_TIM_GET_FLAG(&tim4, TIM_FLAG_UPDATE) != RESET) {
        if (__HAL_TIM_GET_ITSTATUS(&tim4, TIM_IT_UPDATE) != RESET) {
            __HAL_TIM_CLEAR_FLAG(&tim4, TIM_FLAG_UPDATE);
            if (hw_config.callback_tick != NULL) {
                hw_config.callback_tick();
                timer_count++;
                if(timer_count > hw_config.tank_refresh_every_secs) {
                    timer_count = 0;
                    HAL_ADC_Start_IT(&hadc1);
                }
            }
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    for(int i = 0 ; i < BTN_TYPE_TOTAL ; i++) {
        if(hw_config.buttons->pin == GPIO_Pin && hw_config.callback_button != NULL) {
            hw_config.callback_button(i);
        }
    }
}

uint8_t channel = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc1_ref) {
    uint32_t value = HAL_ADC_GetValue(hadc1_ref);
    channel = 0;
    if(channel == 0) {
        if(hw_config.callback_tank_liters != NULL) {
            uint32_t tank_liters = (value - hw_config.tank_step_offset) * hw_config.tank_liters_per_step;
            hw_config.callback_tank_liters(tank_liters);
        }
    } else if(channel == 1) {
        if(hw_config.callback_batt_volts != NULL) {
            hw_config.callback_batt_volts(value * hw_config.millivolt_per_step_batt);
        }
    }
    channel++;
}



/* USER CODE BEGIN 0 */



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


