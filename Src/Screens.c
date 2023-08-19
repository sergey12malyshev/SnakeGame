#include <stdint.h>
#include <stdbool.h>

#include "Screens.h"
#include "SPI_TFT.h"
#include "hard.h"
#include "main.h"

extern const int16_t SWversionMajor, SWversionMinor;
extern const uint16_t green_color, white_color; 


void createFood(uint16_t x0, uint16_t y0, const uint16_t sizeFood)
{
  const uint16_t green = COLOR(0, 255, 0);
  fillCircle(x0, y0, sizeFood, green);
}

void deleteFood(uint16_t x0, uint16_t y0, const uint16_t sizeFood)
{
  const uint16_t black = COLOR(0, 0, 0);
  fillCircle(x0, y0, sizeFood, black);
  scoreIncrement();
}

void createWalls(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  const uint16_t size = 4;
  const uint16_t blue = COLOR(42, 165, 184);
  H_line(x0, y0, x1, y1, size, blue);
}

void createWorkRegion(void)
{
  LCD_Fill(0x0000);
  line(0, 201, 319, 201, 0xFFFF);
  line(0, 0, 0, 199, 0xFFFF);
  STRING_OUT("Score", 10, 210, 1, white_color, 0x0000);
  STRING_OUT("%", 270, 210, 1, green_color, 0x0000);
  batterySumbolShow();
}

void screenSaver(void)
{
  const uint16_t colorBg = COLOR(48, 207, 172);
  LCD_Fill(colorBg);
  STRING_OUT("SNAKE GAME", 65, 100, 7, 0x00FF, colorBg);
  STRING_OUT("Ver.", 100, 220, 5, 0x00FF, colorBg);
  STRING_NUM_L(SWversionMajor, 1, 180, 220, 0x00FF, colorBg);
  STRING_OUT(".", 195, 220, 4, 0x00FF, colorBg);
  STRING_NUM_L(SWversionMinor, 1, 205, 220, 0x00FF, colorBg); 
}

void screenEndGame(void)
{
  const uint16_t colorBg = COLOR(242, 65, 98);
  LCD_Fill(colorBg);
  STRING_OUT("GAME OVER", 85, 100, 3, 0x00FF, colorBg);
  STRING_OUT("press button >", 5, 210, 1, 0x00FF, green_color);
}

void screenGameCompleted(void)
{
  const uint16_t colorBg = COLOR(43, 217, 46);
  LCD_Fill(colorBg);
  STRING_OUT("Good game!", 100, 180, 3, 0x00FF, colorBg);
}

void screenOverVoltageError(void)
{
  const uint16_t colorBg = COLOR(255, 0, 0);
  LCD_Fill(colorBg);
  STRING_OUT("OVERVOLTAGE!", 80, 180, 3, 0xFFFF, colorBg);
}

void screenUnderVoltageError(void)
{
  const uint16_t colorBg = COLOR(255, 0, 0);
  LCD_Fill(colorBg);
  STRING_OUT("UNDERVOLTAGE!", 80, 180, 3, 0xFFFF, colorBg);
}

void batterySumbolShow(void)
{
  const uint8_t x_min = 206;
  const uint8_t y_min = 208;
  const uint8_t y_max = 233;

  line(x_min, y_min, 290, y_min, green_color);
  line(x_min, y_max, 290, y_max, green_color);
  line(x_min, y_min, x_min, y_max, green_color);

  line(290, y_min, 290, y_min + 5, green_color);
  line(290, y_max - 5, 290, y_max, green_color);

  line(290, y_min + 5, 295, y_min + 5, green_color);
  line(290, y_max - 5, 295, y_max - 5, green_color);

  line(295, y_max - 5, 295, y_min + 5, green_color);
}
