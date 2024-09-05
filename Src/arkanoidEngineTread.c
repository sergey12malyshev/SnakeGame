/*
* Arkanoid game machine
* 03.01.24
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#include "main.h"
#include "workState.h"
#include "arkanoidEngineTread.h"
#include "Menu.h"
#include "SPI_TFT.h"
#include "hard.h"
#include "Screens.h"
#include "Sound.h"
#include "colors.h"
#include "time.h"

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"

/* Enable debug mode */
#define DEBUG              false
#define DEBUG_PRINT(...) do { if (DEBUG) sendUART(__VA_ARGS__); } while (0)  

#define X_MIN 1U
#define X_MAX 319U
#define Y_MIN 0U
#define Y_MAX 198U


typedef uint16_t pixel_t;

typedef struct
{
  pixel_t x;
  pixel_t y;
} coordinate;

typedef struct
{
  pixel_t x;
  pixel_t y;
  uint8_t size;
  int8_t changeX;
  int8_t changeY;
} ball;

typedef struct
{
  pixel_t x;
  pixel_t y;
} platform;

/* Init Game variable */

ball ball1 = {0};
platform platform1 = {0};

const pixel_t lengthPlatform = 40;
const pixel_t widthPlatform = 5;

static uint16_t scoreArk = 0;


static void createBall(const pixel_t x, const pixel_t y, const pixel_t size)
{
  fillCircle(x, y, size, getWhite());
}

static void eraseBall(const pixel_t x, const pixel_t y, const pixel_t size)
{
  fillCircle(x, y, size, getBlack());
}

static void createPlatform(const pixel_t x, const pixel_t y)
{
  H_line(x, y, x + lengthPlatform, y, widthPlatform, getWhite());
}

static void erasePlatform(const pixel_t x, const pixel_t y)
{
  H_line(x, y, x + lengthPlatform , y, widthPlatform, getBlack());
}

void arkanoidInitGame(void)
{
  /* Отрисуем рабочее поле */
  createWorkRegion();
  /* Предустановим переменные */
  scoreArk = 0;
  ball1 = (ball){100, 100, 5, 1, -1};
  platform1 = (platform){100, 190};

  scoreUpdate(scoreArk);
  createPlatform(platform1.x, platform1.y);
}

static void arkanoidEndGame(void)
{
  stringGameOver();
  soundGameOver();
  screenEndGame();

  WDT_CLEAR;
  HAL_Delay(300);

  while(true)
  {
    WDT_CLEAR;
    if (buttonLeftHandler())
    {
      screenMainMenu();
      setWorkState(MENU);
      break;
    }
    if (buttonRightHandler())
    {
      arkanoidInitGame();
      break;
    }
  }
}

/*
 * Протопоток ArcanoidgameEngineThread
 * 
 * 
 */
PT_THREAD(ArcanoidGameEngineThread(struct pt *pt))
{
  static uint32_t timerCountGameEngine;

  PT_BEGIN(pt);

  setTime(&timerCountGameEngine);

  while (1)
  {
    PT_DELAY_MS(pt, &timerCountGameEngine, 45U - getSpeedGame());


    if((ball1.x + ball1.size) >= X_MAX)
    {
      ball1.changeX *= -1;
    }
    if((ball1.x - ball1.size) <= X_MIN)
    {
      ball1.changeX *= -1;
    }
    if((ball1.y - ball1.size) <= Y_MIN)
    {
      ball1.changeY *= -1;
    }

    if((ball1.y + ball1.size) >= platform1.y)
    {
      if(((ball1.x + ball1.size) >= platform1.x)&&((ball1.x - ball1.size) <= platform1.x + lengthPlatform))
      {
        ball1.changeY = -1;
        beep(15);
        scoreUpdate(++scoreArk);
      }
      else
      {
        if((ball1.y + ball1.size) >= Y_MAX)
        {
          arkanoidEndGame();
        }
      }
    }

    if (buttonRightHandler())
    {
      if (platform1.x + lengthPlatform <= X_MAX)
      {
        erasePlatform(platform1.x, platform1.y);
        platform1.x += 20;
        createPlatform(platform1.x, platform1.y);
      }
    }
    if (buttonLeftHandler())
    {
      if (platform1.x >= X_MIN)
      {
        erasePlatform(platform1.x, platform1.y);
        platform1.x -= 20;
        createPlatform(platform1.x, platform1.y);
      }
    }

    eraseBall(ball1.x, ball1.y, ball1.size);
    ball1.x += ball1.changeX;
    ball1.y += ball1.changeY;
    createBall(ball1.x, ball1.y, ball1.size);

    PT_YIELD(pt);
  }

  PT_END(pt);
}
