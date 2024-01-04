#include <stdint.h>
#include <stdbool.h>

#include "gameEngineThread.h"
#include "arkanoidEngineTread.h"
#include "monitorThread.h"
#include "SPI_TFT.h"
#include "hard.h"
#include "Screens.h"
#include "Sound.h"
#include "main.h"
#include "Menu.h"
#include "colors.h"
#include "runBootloader.h" 
#include "workState.h"

static uint8_t speedGame = 25;

static const uint16_t colorBg = 0x0000;


void screenMainMenu(void)
{
  LCD_Fill(colorBg);

  STRING_OUT("MENU", 95, 15, 10, getGreen(), colorBg);
  STRING_OUT("START", 90, 50, 7, colorBg, getWhite());
  STRING_OUT("START2", 90, 85, 1, getWhite(), colorBg);
  STRING_OUT("INFO", 90, 120, 7, getWhite(), colorBg);
  STRING_OUT("SETTINGS", 90, 155, 7, getWhite(), colorBg);
  STRING_OUT("UPDATE", 90, 190, 7, getWhite(), colorBg);

  STRING_OUT("^", 10, 210, 1, 0x00FF, getGreen());
  STRING_OUT(">", 290, 210, 1, 0x00FF, getGreen());
}

static void choiceStart(void)
{
  STRING_OUT("START", 90, 50, 7, colorBg, getWhite());
  STRING_OUT("START2", 90, 85, 1, getWhite(), colorBg);
  STRING_OUT("INFO", 90, 120, 7, getWhite(), colorBg);
  STRING_OUT("SETTINGS", 90, 155, 7, getWhite(), colorBg);
  STRING_OUT("UPDATE", 90, 190, 7, getWhite(), colorBg);
}

static void choiceStart2(void)
{
  STRING_OUT("START", 90, 50, 7, getWhite(), colorBg);
  STRING_OUT("START2", 90, 85, 1, colorBg, getWhite());
  STRING_OUT("INFO", 90, 120, 7, getWhite(), colorBg);
  STRING_OUT("SETTINGS", 90, 155, 7, getWhite(), colorBg);
  STRING_OUT("UPDATE", 90, 190, 7, getWhite(), colorBg);
}

static void choiceInfo(void)
{
  STRING_OUT("START", 90, 50, 7, getWhite(), colorBg);
  STRING_OUT("START2", 90, 85, 1, getWhite(), colorBg);
  STRING_OUT("INFO", 90, 120, 7, colorBg, getWhite());
  STRING_OUT("SETTINGS", 90, 155, 7, getWhite(), colorBg);
  STRING_OUT("UPDATE", 90, 190, 7, getWhite(), colorBg);
}

static void choiceSettings(void)
{
  STRING_OUT("START", 90, 50, 7, getWhite(), colorBg);
  STRING_OUT("START2", 90, 85, 1, getWhite(), colorBg);
  STRING_OUT("INFO", 90, 120, 7, getWhite(), colorBg);
  STRING_OUT("SETTINGS", 90, 155, 7, colorBg, getWhite());
  STRING_OUT("UPDATE", 90, 190, 7, getWhite(), colorBg);
}

static void choiceUpdate(void)
{
  STRING_OUT("START", 90, 50, 7, getWhite(), colorBg);
  STRING_OUT("START2", 90, 85, 1, getWhite(), colorBg);
  STRING_OUT("INFO", 90, 120, 7, getWhite(), colorBg);
  STRING_OUT("SETTINGS", 90, 155, 7, getWhite(), colorBg);
  STRING_OUT("UPDATE", 90, 190, 7, colorBg, getWhite());
}

static void InfoScreen(void)
{
  extern const int16_t SWversionMajor, SWversionMinor, SWversionPatch;
  

  LCD_Fill(colorBg);
  STRING_OUT("PAC-MAN game", 25, 10, 5, getOrange(), colorBg);

  uint8_t start_x = 5, start_y = 55;
  STRING_OUT("Version:", start_x, start_y, 5, getWhite(), colorBg);
  STRING_NUM_L(SWversionMajor, 1, start_x+80+15+70, start_y, getWhite(), colorBg);
  STRING_OUT(".", start_x+80+15+15+70, start_y, 4, getWhite(), colorBg);
  STRING_NUM_L(SWversionMinor, 2,  start_x+80+15+10+20+70, start_y, getWhite(), colorBg);
  STRING_OUT(".", start_x+80+15+35+30+70, start_y, 4, getWhite(), colorBg);
  STRING_NUM_L(SWversionPatch, 2, start_x+80+15+52+30+70, start_y, getWhite(), colorBg);

  STRING_OUT(__DATE__, start_x, 105, 5, getWhite(), colorBg);   //https://spec-zone.ru/gcc~9_cpp/standard-predefined-macros
  STRING_OUT(__TIME__, start_x+100, 140, 5, getWhite(), colorBg);

  simple_font_string_OUT("github.com/sergey12malyshev", 100, 200, 1, getWhite(), colorBg);

  STRING_OUT("<", 10, 210, 1, 0x00FF, getGreen());
}

static void speedMenuUpdate(uint8_t speedGameLoc)
{
  
  STRING_NUM_L(speedGameLoc, 2, 155, 80, getOrange(), colorBg);
}

static void settingsScreen(void)
{
  
  LCD_Fill(colorBg);
  STRING_OUT("Settings", 65, 20, 5, getWhite(), colorBg);

  STRING_OUT("Speed:", 25 , 80, 5, getWhite(), colorBg);
  speedMenuUpdate(speedGame);

  STRING_OUT("<", 10, 210, 1, 0x00FF, getGreen());
  STRING_OUT("^", 290, 210, 1, 0x00FF, getGreen());
}

uint8_t getSpeedGame(void)
{
  return speedGame;
}

void setSpeedGame(const uint8_t s)
{
  speedGame = s;
}


bool mainMenu(void)
{
  enum MENU_ITEMS {START = 0, START2, INFO, SETTINGS, UPDATE};
  static uint8_t count = START;  

    if (buttonLeftHandler())
    {
      beep(1);
      count++;
      if (count > UPDATE) count = START;

      switch (count)
      {
        case START:  
          choiceStart();
          break;
        case START2:  
          choiceStart2();
          break;
        case INFO: 
          choiceInfo(); 
          break;
        case SETTINGS: 
          choiceSettings(); 
          break; 
        case UPDATE: 
          choiceUpdate(); 
          break; 
        default:
          assert_param(0); // Error
          break;
      }
    }
    
    if (buttonRightHandler())
    {   
      beep(1);

      switch (count)
      {
        case START:  
          initGame();
          setWorkState(GAME1);
          break;
        case START2:
          arkanoidInitGame();
          setWorkState(GAME2);
          break;
        case INFO: 
          InfoScreen();
          while((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET)) WDT_CLEAR;
          screenMainMenu();
          count = START; 
          break;
        case SETTINGS: 
          settingsScreen();
          while((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET))
          {
            WDT_CLEAR;
            if(buttonRightHandler())
            {
              speedGame += 5;
              if(speedGame > 45) speedGame = 5;
              speedMenuUpdate(speedGame);
              uint32_t rc = flash_write(flash_get_page(), speedGame);
              if(rc) sendUART((uint8_t *)"Flash write error\r\n");
            }
          }
          screenMainMenu();
          count = START; 
          break; 
        case UPDATE:  
          runBootloader();
        default:
          assert_param(0); // Error
          break;
      }
    }
  
  return true;
}


