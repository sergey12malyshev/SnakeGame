#include <stdint.h>
#include <stdbool.h>

#include "Screens.h"
#include "SPI_TFT.h"
#include "hard.h"
#include "Screens.h"
#include "main.h"
#include "colors.h"

/*X0*******************
 Y0
 *
 *
 *******************/

const unsigned char objets[][8]=
{
  {0x7C,0x02,0xC9,0x01,0x01,0xC9,0x02,0x7C}, // monster
  {0x00,0x42,0xA5,0x99,0x81,0x81,0x42,0x3C}  // Pac-men
};

void createMonster(uint8_t object_number, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor, uint16_t bgcolor)
{ // медленный алгоритм вывода
	for (int i = 0; i < 8; i++)
	{
		unsigned char temp = objets[object_number][i];

		for (unsigned char f = 0; f < 8; f++)
		{
			if ((temp >> f) & 0x01)
			{
				LCD_fillRect(x0 + i * size, y0 + f * size, size, size, fgcolor);
			}
			else
			{
				LCD_fillRect(x0 + i * size, y0 + f * size, size, size, bgcolor);
			}
		}
	}
}

void disableMonster(uint16_t x0, uint16_t y0, uint16_t size)
{ // медленный алгоритм закрашивания
	for (int i = 0; i < 8; i++)
	{
		for (unsigned char f = 0; f < 8; f++)
		{
			LCD_fillRect(x0 + i * size, y0 + f * size, size, size, getBlack());
		}
	}
}

void createFood(uint16_t x0, uint16_t y0, const uint16_t sizeFood)
{
  fillCircle(x0, y0, sizeFood, getGreen());
}

void deleteFood(uint16_t x0, uint16_t y0, const uint16_t sizeFood)
{
  fillCircle(x0, y0, sizeFood, getBlack());
  scoreIncrement();
}

void createWalls(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  const uint16_t size = 4;
  H_line(x0, y0, x1, y1, size, getBlue());
}

void createWorkRegion(void)
{
  LCD_Fill(getBlack());
  line(0, 201, 319, 201, getWhite());
  line(0, 0, 0, 199, getWhite());
  STRING_OUT("Score", 10, 210, 1, getWhite(), getBlack());
  batterySumbolShow();
}

void screenSaver(void)
{
  extern const int16_t SWversionMajor, SWversionMinor;
  const uint16_t colorBg = getBlack();
  LCD_Fill(colorBg);
  STRING_OUT("PAC-MAN", 90, 110, 10, getBlue(), colorBg);
  STRING_OUT("Ver.", 100, 220, 5, 0x00FF, colorBg);
  STRING_NUM_L(SWversionMajor, 1, 180, 220, 0x00FF, colorBg);
  STRING_OUT(".", 195, 220, 4, 0x00FF, colorBg);
  STRING_NUM_L(SWversionMinor, 1, 205, 220, 0x00FF, colorBg); 

  createMonster(0, 50, 50, 5, getRed(), colorBg);
  createMonster(1, 200, 150, 5, getOrange(), colorBg);
  createMonster(0, 195, 45, 5, getBlack(), COLOR(0, 0, 255));
}

void screenEndGame(void)
{
  const uint16_t colorBg = COLOR(242, 65, 98);
  LCD_Fill(colorBg);
  STRING_OUT("GAME OVER", 85, 100, 3, 0x00FF, colorBg);
  STRING_OUT(">", 290, 210, 1, 0x00FF, getGreen());
}

void screenGameCompleted(void)
{
  const uint16_t colorBg = COLOR(43, 217, 46);
  LCD_Fill(colorBg);
  STRING_OUT("Next Level!", 100, 140, 3, 0x00FF, colorBg);
  STRING_OUT(">", 290, 210, 1, 0x00FF, getGreen());
}

void screenOverVoltageError(void)
{
  LCD_Fill(getRed());
  STRING_OUT("OVERVOLTAGE!", 80, 180, 3, getWhite(), getRed());
}

void screenUnderVoltageError(void)
{
  LCD_Fill(getRed());
  STRING_OUT("UNDERVOLTAGE!", 80, 180, 3, getWhite(), getRed());
}

void batterySumbolShow(void)
{
  const uint8_t x_min = 206;
  const uint8_t y_min = 208;
  const uint8_t y_max = 233;

  line(x_min, y_min, 290, y_min, getGreen());
  line(x_min, y_max, 290, y_max, getGreen());
  line(x_min, y_min, x_min, y_max, getGreen());

  line(290, y_min, 290, y_min + 5, getGreen());
  line(290, y_max - 5, 290, y_max, getGreen());

  line(290, y_min + 5, 295, y_min + 5, getGreen());
  line(290, y_max - 5, 295, y_max - 5, getGreen());

  line(295, y_max - 5, 295, y_min + 5, getGreen());
  STRING_OUT("%", 270, 210, 1, getGreen(), getBlack());
}
