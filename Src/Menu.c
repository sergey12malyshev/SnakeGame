#include <stdint.h>
#include <stdbool.h>

#include "gameEngineThread.h"
#include "SPI_TFT.h"
#include "hard.h"
#include "Screens.h"
#include "Sound.h"
#include "main.h"
#include "Menu.h"
#include "colors.h"

static bool menuEnabled = true;

bool getMenuState(void)
{
  return menuEnabled;
}

void setMenuState(const bool state)
{
  menuEnabled = state;
}

void screenMainMenu(void)
{
  const uint16_t colorBg = 0x0000;
  LCD_Fill(colorBg);

  STRING_OUT("MENU", 95, 15, 10, getGreen(), colorBg);
  STRING_OUT("START", 90, 70, 7, colorBg, getWhite());
  STRING_OUT("INFO", 90, 120, 7, getWhite(), colorBg);

  STRING_OUT("^", 10, 210, 1, 0x00FF, getGreen());
  STRING_OUT(">", 290, 210, 1, 0x00FF, getGreen());
}

static void choiceStart(void)
{
  const uint16_t colorBg = 0x0000;

  STRING_OUT("START", 90, 70, 7, colorBg, getWhite());
  STRING_OUT("INFO", 90, 120, 7, getWhite(), colorBg);
}

static void choiceInfo(void)
{
  const uint16_t colorBg = 0x0000;

  STRING_OUT("START", 90, 70, 7, getWhite(), colorBg);
  STRING_OUT("INFO", 90, 120, 7, colorBg, getWhite());
}

static void InfoScreen(void)
{
  extern const int16_t SWversionMajor, SWversionMinor;
  const uint16_t colorBg = 0x0000;
  LCD_Fill(colorBg);
  STRING_OUT("PAC-MAN game", 25, 20, 5, getWhite(), colorBg);

  uint8_t start_x = 5, start_y = 65;
  STRING_OUT("Ver.", start_x, start_y, 5, getWhite(), colorBg);
  STRING_NUM_L(SWversionMajor, 1, start_x+80+15, start_y, getWhite(), colorBg);
  STRING_OUT(".", start_x+80+15+15, start_y, 4, getWhite(), colorBg);
  STRING_NUM_L(SWversionMinor, 1,  start_x+80+15+10+20, start_y, getWhite(), colorBg);

  STRING_OUT("sergey12malyshev", 5, 120, 1, getWhite(), colorBg);
  STRING_OUT(__DATE__, start_x, 165, 5, getWhite(), colorBg);   //https://spec-zone.ru/gcc~9_cpp/standard-predefined-macros
  STRING_OUT(__TIME__, start_x+100, 200, 5, getWhite(), colorBg);

  STRING_OUT("<", 10, 210, 1, 0x00FF, getGreen());
}

bool mainMenu(void)
{
  static uint8_t count = 0;  

    if (buttonLeftHandler())
    {
      beep(0);
      count++;
      if (count > 1) count = 0;

      switch (count)
      {
        case 0:  
          choiceStart();
          break;
        case 1: 
          choiceInfo(); 
          break;
        
        default:
          break;
      }
    }
    
    if (buttonRightHandler())
    {   
      beep(0);

      switch (count)
      {
        case 0:  
          initGame();
          return false;
        case 1: 
          InfoScreen();
          while((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET));
          screenMainMenu();
          count = 0; 
          break;
        
        default:
          break;
      }
    }
  
  return true;
}

