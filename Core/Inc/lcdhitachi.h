//
// Created by gaspar on 22/10/20.
//

#ifndef FSM_PUMP_CONTROLLER_V1_LCDHITACHI_H
#define FSM_PUMP_CONTROLLER_V1_LCDHITACHI_H

#include "stm32f4xx_hal.h"

uint8_t LCD_init(I2C_HandleTypeDef*);
void LCD_setCursor(uint8_t row, uint8_t col);
void LCD_1stLine(void);
void LCD_2ndLine(void);
void LCD_TwoLines(void);
void LCD_OneLine(void);
void LCD_cursorShow(uint8_t state);
void LCD_clear(void);
void LCD_display(uint8_t state);
void LCD_shiftRight(uint8_t offset);
void LCD_shiftLeft(uint8_t offset);
void LCD_printf(const uint8_t* str, ...);

#endif //FSM_PUMP_CONTROLLER_V1_LCDHITACHI_H
