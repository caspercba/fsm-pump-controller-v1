//
// Created by gaspar on 22/10/20.
//

#include <stdarg.h>
#include <stdio.h>
#include <memory.h>
#include "lcdhitachi.h"
#include "stm32f4xx_hal_i2c.h"

#define LD4_Pin GPIO_PIN_12


static void Error_Handler() {};


/* LCD Commands */
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

/* Commands bitfields */
//1) Entry mode Bitfields
#define LCD_ENTRY_SH      0x01
#define LCD_ENTRY_ID      0x02
//2) Display control
#define LCD_DISPLAY_B     0x01
#define LCD_DISPLAY_C     0x02
#define LCD_DISPLAY_D     0x04
//3) Shift control
#define LCD_SHIFT_RL      0x04
#define LCD_SHIFT_SC      0x08
//4) Function set control
#define LCD_FUNCTION_F    0x04
#define LCD_FUNCTION_N    0x08
#define LCD_FUNCTION_DL   0x10

/* I2C Control bits */
#define LCD_RS        (1 << 0)
#define LCD_RW        (1 << 1)
#define LCD_EN        (1 << 2)
#define LCD_BK_LIGHT  (1 << 3)

/* Library variables */
static I2C_HandleTypeDef* LCDHandle;
static uint8_t LCD_I2C_SLAVE_ADDRESS=0;
#define LCD_I2C_SLAVE_ADDRESS_0  0x4E
#define LCD_I2C_SLAVE_ADDRESS_1  0x7E

static void LCD_sendCommand(uint8_t command)
{
    const uint8_t command_0_3 = (0xF0 & (command<<4));
    const uint8_t command_4_7 = (0xF0 & command);
    uint8_t i2cData[4] =
            {
                    command_4_7 | LCD_EN | LCD_BK_LIGHT,
                    command_4_7 | LCD_BK_LIGHT,
                    command_0_3 | LCD_EN | LCD_BK_LIGHT,
                    command_0_3 | LCD_BK_LIGHT,
            };
    HAL_I2C_Master_Transmit(LCDHandle, LCD_I2C_SLAVE_ADDRESS, i2cData, 4, 200);
}

static void LCD_sendData(uint8_t data)
{
    const uint8_t data_0_3 = (0xF0 & (data<<4));
    const uint8_t data_4_7 = (0xF0 & data);
    uint8_t i2cData[4] =
            {
                    data_4_7 | LCD_EN | LCD_BK_LIGHT | LCD_RS,
                    data_4_7 | LCD_BK_LIGHT | LCD_RS,
                    data_0_3 | LCD_EN | LCD_BK_LIGHT | LCD_RS,
                    data_0_3 | LCD_BK_LIGHT | LCD_RS,
            };
    HAL_I2C_Master_Transmit(LCDHandle, LCD_I2C_SLAVE_ADDRESS, i2cData, 4, 200);
}

uint8_t LCD_init(I2C_HandleTypeDef *pI2cHandle)
{
    HAL_Delay(50);
    LCDHandle = pI2cHandle;
    if(HAL_I2C_IsDeviceReady(LCDHandle, LCD_I2C_SLAVE_ADDRESS_0, 5, 500) != HAL_OK)
    {
        if(HAL_I2C_IsDeviceReady(LCDHandle, LCD_I2C_SLAVE_ADDRESS_1, 5, 500) != HAL_OK)
        {
            return 1;
        }
        else
        {
            LCD_I2C_SLAVE_ADDRESS = LCD_I2C_SLAVE_ADDRESS_1;
        }
    }
    else
    {
        LCD_I2C_SLAVE_ADDRESS = LCD_I2C_SLAVE_ADDRESS_0;
    }
    //Initialise LCD for 4-bit operation
    //1. Wait at least 15ms
    HAL_Delay(45);
    //2. Attentions sequence
    LCD_sendCommand(0x30);
    HAL_Delay(5);
    LCD_sendCommand(0x30);
    HAL_Delay(1);
    LCD_sendCommand(0x30);
    HAL_Delay(8);
    LCD_sendCommand(0x20);
    HAL_Delay(8);

    LCD_sendCommand(LCD_FUNCTIONSET | LCD_FUNCTION_N);
    HAL_Delay(1);
    LCD_sendCommand(LCD_DISPLAYCONTROL);
    HAL_Delay(1);
    LCD_sendCommand(LCD_CLEARDISPLAY);
    HAL_Delay(3);
    LCD_sendCommand(0x04 | LCD_ENTRY_ID);
    HAL_Delay(1);
    LCD_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_D);
    HAL_Delay(3);

    return 1;
}

void LCD_setCursor(uint8_t row, uint8_t col)
{
    uint8_t maskData;
    maskData = (col)&0x0F;
    if(row==0)
    {
        maskData |= (0x80);
        LCD_sendCommand(maskData);
    }
    else
    {
        maskData |= (0xc0);
        LCD_sendCommand(maskData);
    }
}

void LCD_1stLine(void)
{
    LCD_setCursor(0,0);
}

void LCD_2ndLine(void)
{
    LCD_setCursor(1,0);
}

void LCD_TwoLines(void)
{
     LCD_sendCommand(LCD_FUNCTIONSET | LCD_FUNCTION_N);
}
void LCD_OneLine(void)
{
    LCD_sendCommand(LCD_FUNCTIONSET);
}

void LCD_cursorShow(uint8_t state)
{
    if(state)
    {
        LCD_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_B | LCD_DISPLAY_C | LCD_DISPLAY_D);
    }
    else
    {
        LCD_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_D);
    }
}

void LCD_clear(void)
{
    LCD_sendCommand(LCD_CLEARDISPLAY);
    HAL_Delay(3);
}

void LCD_display(uint8_t state)
{
    if(state)
    {
        LCD_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_B | LCD_DISPLAY_C | LCD_DISPLAY_D);
    }
    else
    {
        LCD_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_B | LCD_DISPLAY_C);
    }
}

void LCD_shiftRight(uint8_t offset)
{
    for(uint8_t i=0; i<offset;i++)
    {
        LCD_sendCommand(0x1c);
    }
}

void LCD_shiftLeft(uint8_t offset)
{
    for(uint8_t i=0; i<offset;i++)
    {
        LCD_sendCommand(0x18);
    }
}

void LCD_printf(const uint8_t* str, ...)
{

    char stringArray[20];
    va_list args;
    va_start(args, str);
    vsprintf(stringArray, str, args);
    va_end(args);
    for(uint8_t i=0;  i<strlen(stringArray) && i<16; i++)
    {
        LCD_sendData((uint8_t)stringArray[i]);
    }

}

