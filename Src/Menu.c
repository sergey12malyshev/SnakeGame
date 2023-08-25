#include <stdint.h>
#include <stdbool.h>

#include "gameEngineThread.h"
#include "SPI_TFT.h"
#include "hard.h"
#include "Screens.h"
#include "Sound.h"
#include "main.h"
#include "Menu.h"

extern const uint16_t orange_color;
extern const uint16_t red_color;
extern const uint16_t blue_color;
extern const uint16_t green_color, white_color; 

static void screenMainMenu(void)
{
  const uint16_t colorBg = 0x0000;
  LCD_Fill(colorBg);

  STRING_OUT("MENU", 95, 15, 10, green_color, colorBg);
  STRING_OUT("START", 90, 70, 7, colorBg, white_color);
  STRING_OUT("INFO", 90, 120, 7, white_color, colorBg);

  STRING_OUT("^", 10, 210, 1, 0x00FF, green_color);
  STRING_OUT(">", 290, 210, 1, 0x00FF, green_color);
}

static void choiceStart(void)
{
  const uint16_t colorBg = 0x0000;

  STRING_OUT("START", 90, 70, 7, colorBg, white_color);
  STRING_OUT("INFO", 90, 120, 7, white_color, colorBg);
}

static void choiceInfo(void)
{
  const uint16_t colorBg = 0x0000;

  STRING_OUT("START", 90, 70, 7, white_color, colorBg);
  STRING_OUT("INFO", 90, 120, 7, colorBg, white_color);
}

static void InfoScreen(void)
{
  const uint16_t colorBg = 0x0000;
  LCD_Fill(colorBg);
  STRING_OUT("sergey12malyshev", 5, 60, 1, white_color, colorBg);
  STRING_OUT("2023", 90, 120, 1, white_color, colorBg);
  STRING_OUT("<", 10, 210, 1, 0x00FF, green_color);
}

static bool buttonLeftHandler(void)
{
  static bool flagBut1 = false;
  bool rc = false;

  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) && !flagBut1)
  { // обработчик нажатия
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET)
    {
      flagBut1 = true;
      rc = true;
    }
  }
  
  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET) && flagBut1)
  { // обработчик отпускания
    flagBut1 = false;
  }
  
  return rc;
}

static bool buttonRightHandler(void)
{
  static bool flagBut1 = false;
  bool rc = false;

  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET) && !flagBut1)
  { // обработчик нажатия
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET)
    {
      flagBut1 = true;
      rc = true;
    }
  }
  
  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET) && flagBut1)
  { // обработчик отпускания
    flagBut1 = false;
  }
  
  return rc;
}

void mainMenu(void)
{
  uint8_t count = 0;  
  screenMainMenu();

  while(true)
  {
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
              return;

            break;
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
  }
}


