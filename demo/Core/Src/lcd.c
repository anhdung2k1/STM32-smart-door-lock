#include "lcd.h"
#include "main.h"


void LCD_Delay(void);
void LCD_Write4Bits(uint8_t nb);
void LCD_PulseEnable(void);

void LCD_Delay(void)
{
  uint16_t i;
  for (i = 0; i < 1000; i++) {}
}

void LCD_PulseEnable(void)
{
  EN_RESET();
  LCD_Delay();
  EN_SET();
  LCD_Delay();
  EN_RESET();
  HAL_Delay(1);
}

void LCD_Write4Bits(uint8_t nb)
{
  if (((nb >> 0) & 0x01) == 1) { D4_SET(); } else { D4_RESET(); }
  if (((nb >> 1) & 0x01) == 1) { D5_SET(); } else { D5_RESET(); }
  if (((nb >> 2) & 0x01) == 1) { D6_SET(); } else { D6_RESET(); }
  if (((nb >> 3) & 0x01) == 1) { D7_SET(); } else { D7_RESET(); }
  LCD_PulseEnable();
}

void LCD_Command(uint8_t cmd)
{
  RS_RESET();

  LCD_Write4Bits(cmd >> 4);
  LCD_Write4Bits(cmd);
}

void LCD_Data(uint8_t dt)
{
  RS_SET();
 
  LCD_Write4Bits(dt >> 4);
  LCD_Write4Bits(dt);
}

void LCD_Char(char chr)
{
  LCD_Data((uint8_t) chr);
}

void LCD_String(char * str)
{
  uint8_t i = 0;
  while (str[i] != 0)
  {
   LCD_Char(str[i]);
   i++;
  }
}

void LCD_SetPos(uint8_t x, uint8_t y)
{
  uint8_t firstChar[] = {0x80, 0xC0, 0x94, 0xD4};
  LCD_Command(firstChar[y] + x);
}

void LCD_Clear(void)
{
  LCD_Command(0x01);
  HAL_Delay(2);
}

void LCD_Init(void)
{
  HAL_Delay(50);
  RS_RESET();
  EN_RESET();
 
  LCD_Write4Bits(0x03);
  HAL_Delay(5);
  LCD_Write4Bits(0x03);
  HAL_Delay(5);
  LCD_Write4Bits(0x03);
  HAL_Delay(1);
  LCD_Write4Bits(0x02);
  LCD_Command(0x28);
  LCD_Command(0x0C);
  LCD_Clear();
  LCD_Command(0x06);
}

