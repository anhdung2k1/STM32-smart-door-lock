#include "stdint.h"
#include "main.h"

#ifndef LCD_H_
#define LCD_H_


#define RS_SET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
#define EN_SET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
#define D4_SET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
#define D5_SET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
#define D6_SET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
#define D7_SET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);

#define RS_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
#define EN_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
#define D4_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
#define D5_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
#define D6_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
#define D7_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

void LCD_Command(uint8_t cmd);
void LCD_Data(uint8_t dt);
void LCD_Char(char chr);
void LCD_String(char * str);
void LCD_SetPos(uint8_t x, uint8_t y);
void LCD_Clear(void);
void LCD_Init(void);

#endif

