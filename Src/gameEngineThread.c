#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "SPI_TFT.h"
#include "hard.h"
#include "Screens.h"
#include "Sound.h"
#include "gameEngineThread.h"
#include "colors.h"

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"

#define DEBUG              false
#define NO_WALS_DEATH      false

#define X_MIN 1U
#define X_MAX 319U
#define Y_MIN 0U
#define Y_MAX 198U

static struct pt gameEngine_pt;

typedef enum
{
  NONE = 0,
  UP,
  LEFT,
  DOWN,
  RIGHT
} SPACE_ENUM;

SPACE_ENUM space = UP;

static uint8_t level = 0;

const uint16_t sizePacMan = 2;
static int16_t x_PacMan, y_PacMan;
static int16_t old_x = 0, old_y = 0;

static int8_t changeX = 0; // changes the direction of the PacMan
static int8_t changeY = -1;

static int16_t score = 0, oldScore = 0;
/* https://colorscheme.ru/color-converter.html */

/* Параметры еды: */
const uint8_t quantityFood = 4;

typedef struct
{
  int16_t x;
  int16_t y;
  int8_t size;
  bool disable;
} food;

food food1 = {0}; 
food food2 = {0};
food food3 = {0};
food food4 = {0};

/* Параметры стен: */
typedef struct
{
  uint8_t x1;
  uint8_t y1;
  uint8_t x2;
  uint8_t y2;
} wals;

wals wals1 = {0}; 
wals wals2 = {0};
wals wals3 = {0};
wals wals4 = {0};

typedef struct
{
  uint8_t type;
  int16_t x;
  int16_t y;
  int8_t size;
  uint16_t color;
} monster;

monster monster1 = {0}; 
monster monster2 = {0};


static bool checkWalls(void)
{
  bool rc = false;

  rc |= (((x_PacMan <= (wals1.x1)) && (x_PacMan >= (wals1.x2))) && ((y_PacMan <= (wals1.y1)) && (y_PacMan >= (wals1.y2))));
  rc |= (((x_PacMan <= (wals2.x1)) && (x_PacMan >= (wals2.x2))) && ((y_PacMan <= (wals2.y1)) && (y_PacMan >= (wals2.y2))));
  rc |= (((x_PacMan <= (wals3.x1)) && (x_PacMan >= (wals3.x2))) && ((y_PacMan <= (wals3.y1)) && (y_PacMan >= (wals3.y2))));
  rc |= (((x_PacMan <= (wals4.x1)) && (x_PacMan >= (wals4.x2))) && ((y_PacMan <= (wals4.y1)) && (y_PacMan >= (wals4.y2))));

  return rc;
}

static void scoreUpdate(uint16_t scoreLoc)
{
  STRING_NUM_L(scoreLoc, 2, 120, 210, getWhite(), getBlack());  
}

void scoreIncrement(void)
{
  score += 1;
  beep(10);
}

static inline void up(void)
{
  changeX = 0; // changes the direction of the PacMan
  changeY = -1;
}

static inline void down(void)
{
  changeX = 0;
  changeY = 1;
}

static inline void left(void)
{
  changeX = -1;
  changeY = 0;
}

static inline void right(void)
{
  changeX = 1;
  changeY = 0;
}

static void direction(void)
{
  switch (space)
  {
  case UP:
    up();
    break;
  case LEFT:
    left();
    break;
  case DOWN:
    down();
    break;
  case RIGHT:
    right();
    break;

  default:
    space = UP;
    break;
  }
}

static void pollingButton(void)
{
  if (buttonRightHandler())
  {
    space--;
    if (space == 0)
    {
      space = RIGHT;
    }
    direction();
  }
  if (buttonLeftHandler())
  {
    space++;
    if (space >= 5)
    {
      space = UP;
    }
    direction();
  }
}

static void levelOne(void)
{
  food1 = (food){50, 100, 11, false}; 
  food2 = (food){280, 25, 8, false};  //Составной литерал C99 http://zonakoda.ru/sostavnye-literaly-v-c99.html
  food3 = (food){125, 175, 9, false};
  food4 = (food){235, 180, 12, false};

  wals1 = (wals){80, 180, 80, 20}; 
  wals2 = (wals){165, Y_MAX, 165, 110};
  wals3 = (wals){165, 90, 165, Y_MIN};
  wals4 = (wals){250, 90, 250, Y_MIN};

  monster1 = (monster){0, 50, 50, 3, COLOR(255, 0, 0)};
  monster2 = (monster){0, 210, 30, 3, COLOR(0, 0, 255)};

  /* Отрисуем еду */
  createFood(food1.x, food1.y, food1.size);
  createFood(food2.x, food2.y, food2.size);
  createFood(food3.x, food3.y, food3.size);
  createFood(food4.x, food4.y, food4.size);

  /* Отрисуем препятствия */
  createWalls(wals1.x1, wals1.y1, wals1.x2, wals1.y2);
  createWalls(wals2.x1, wals2.y1, wals2.x2, wals2.y2);
  createWalls(wals3.x1, wals3.y1, wals3.x2, wals3.y2);
  createWalls(wals4.x1, wals4.y1, wals4.x2, wals4.y2); 

  /* Отрисуем монстов */
  createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
  createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
}

static void levelTwo(void)
{
  food1 = (food){20, 100, 4, false}; 
  food2 = (food){200, 10, 6, false};
  food3 = (food){100, 160, 12, false};
  food4 = (food){280, 135, 7, false};

  wals1 = (wals){40, 180, 40, 20}; 
  wals2 = (wals){150, Y_MAX, 150, 110};
  wals3 = (wals){185, 120, 185, 5};
  wals4 = (wals){255, 160, 255, 40};

  monster1 = (monster){0, 50, 15, 3, COLOR(255, 0, 0)};
  monster2 = (monster){0, 210, 165, 3, COLOR(0, 0, 255)};
  /* Отрисуем еду */
  createFood(food1.x, food1.y, food1.size);
  createFood(food2.x, food2.y, food2.size);
  createFood(food3.x, food3.y, food3.size);
  createFood(food4.x, food4.y, food4.size);

  /* Отрисуем препятствия */
  createWalls(wals1.x1, wals1.y1, wals1.x2, wals1.y2);
  createWalls(wals2.x1, wals2.y1, wals2.x2, wals2.y2);
  createWalls(wals3.x1, wals3.y1, wals3.x2, wals3.y2);
  createWalls(wals4.x1, wals4.y1, wals4.x2, wals4.y2); 
}

static void levelThree(void)
{
  food1 = (food){20, 100, 4, false}; 
  food2 = (food){240, 10, 6, false};
  food3 = (food){100, 160, 12, false};
  food4 = (food){280, 135, 7, false};

  wals1 = (wals){140, 188, 140, 0}; 
  wals2 = (wals){160, Y_MAX, 160, 10};
  wals3 = (wals){180, 188, 180, 0};
  wals4 = (wals){200, Y_MAX, 200, 10};

  monster1 = (monster){0, 50, 100, 3, COLOR(255, 0, 0)};
  monster2 = (monster){0, 220, 40, 3, COLOR(0, 0, 255)};

  /* Отрисуем еду */
  createFood(food1.x, food1.y, food1.size);
  createFood(food2.x, food2.y, food2.size);
  createFood(food3.x, food3.y, food3.size);
  createFood(food4.x, food4.y, food4.size);

  /* Отрисуем препятствия */
  createWalls(wals1.x1, wals1.y1, wals1.x2, wals1.y2);
  createWalls(wals2.x1, wals2.y1, wals2.x2, wals2.y2);
  createWalls(wals3.x1, wals3.y1, wals3.x2, wals3.y2);
  createWalls(wals4.x1, wals4.y1, wals4.x2, wals4.y2); 
}

void initGame(void)
{
  /* Отрисуем рабочее поле */
  createWorkRegion();
  /* Предустановим переменные */
  up();
  oldScore = score = 0;
  scoreUpdate(score);
  x_PacMan = 215;
  y_PacMan = 145;
  old_x = 0;
  old_y = 0;
  food1.disable = false;
  food2.disable = false;
  food3.disable = false;
  food4.disable = false;

  switch (level)
  {
    case 0:
      levelOne();
      break;
    case 1:
      levelTwo();
      break;
    case 2:
      levelThree(); 
    default:
      break;
  }
}

static void endGame(void)
{
  HAL_Delay(300);
  while ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET));
  initGame();
}

static void levelUp(void)
{
  level++;
  if(level > 2)
  {
    level = 0;
  }
}

static void levelReset(void)
{
  level = 0;
}

static bool foodIntakeCheck1(void)
{
   return (((x_PacMan <= (food1.x + food1.size)) && (x_PacMan >= (food1.x - food1.size))) && ((y_PacMan <= (food1.y + food1.size)) && (y_PacMan >= (food1.y - food1.size))));
}

static bool foodIntakeCheck2(void)
{
  return (((x_PacMan <= (food2.x + food2.size)) && (x_PacMan >= (food2.x - food2.size))) && ((y_PacMan <= (food2.y + food2.size)) && (y_PacMan >= (food2.y - food2.size))));
}

static bool foodIntakeCheck3(void)
{
  return (((x_PacMan <= (food3.x + food3.size)) && (x_PacMan >= (food3.x - food3.size))) && ((y_PacMan <= (food3.y + food3.size)) && (y_PacMan >= (food3.y - food3.size))));
}

static bool foodIntakeCheck4(void)
{
  return (((x_PacMan <= (food4.x + food4.size)) && (x_PacMan >= (food4.x - food4.size))) && ((y_PacMan <= (food4.y + food4.size)) && (y_PacMan >= (food4.y - food4.size))));
}  

static bool monsterCheck1(void)
{
#if 1
  uint8_t realSize = monster1.size*8U;
  return (((x_PacMan <= (monster1.x + realSize)) && (x_PacMan >= monster1.x)) && ((y_PacMan <= (monster1.y + realSize)) && (y_PacMan >= monster1.y)));
#endif
}
static bool monsterCheck2(void)
{
#if 1
  uint8_t realSize = monster1.size*8U;
  return (((x_PacMan <= (monster2.x + realSize)) && (x_PacMan >= monster2.x)) && ((y_PacMan <= (monster2.y + realSize)) && (y_PacMan >= monster2.y)));
#endif
}

static void PacManUpdateProcess(void)
{
  if (((x_PacMan <= (old_x + sizePacMan)) || (x_PacMan >= (old_x - sizePacMan))) && ((y_PacMan <= (old_y + sizePacMan)) && (y_PacMan >= (old_y - sizePacMan))))
  { // update PacMan
    fillCircle(old_x, old_y, 2, getBlack());
    fillCircle(x_PacMan, y_PacMan, 2, getYellow());
  }
} 

/*
 * Протопоток gameEngineThread
 *
 * 
 */
static PT_THREAD(GameEngineThread(struct pt *pt))
{
  static uint32_t timeCountGameEngine = 0;
  static uint8_t i;

  PT_BEGIN(pt);

  while (1)
  {

    PT_WAIT_UNTIL(pt, (HAL_GetTick() - timeCountGameEngine) > 15);
    timeCountGameEngine = HAL_GetTick();	

    old_x = x_PacMan;
    old_y = y_PacMan;

    y_PacMan = y_PacMan + changeY;
    x_PacMan = x_PacMan + changeX;

    // Захардкодили перемещение монстров
    i++;
    if (i == 90U)
    {
      disableMonster(monster1.x, monster1.y, monster1.size);
      disableMonster(monster2.x, monster2.y, monster2.size);
      monster1.y += 10U;
      monster2.x += 5U;
      createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
      createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
      i = 0U;
    }
    else if(i == 45U)
    {
      disableMonster(monster1.x, monster1.y, monster1.size);
      disableMonster(monster2.x, monster2.y, monster2.size);
      monster1.y -= 10U;
      monster2.x -= 5U;
      createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
      createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
    }

#if NO_WALS_DEATH
    if (y_PacMan > Y_MAX)
    {
      y_PacMan = Y_MIN;
    }
    if (x_PacMan > X_MAX)
    {
      x_PacMan = X_MIN;
    }
    if (y_PacMan < Y_MIN)
    {
      y_PacMan = Y_MAX;
    }
    if (x_PacMan < X_MIN)
    {
      x_PacMan = X_MAX;
    }
#else
    if ((y_PacMan > Y_MAX)||(x_PacMan > X_MAX)||(y_PacMan < Y_MIN)||(x_PacMan < X_MIN))
    {
      screenEndGame();
      soundGameOver();
      levelReset();
      endGame();
    }
#endif

    PacManUpdateProcess();

    if (foodIntakeCheck1())
    { // food 1
      if(!food1.disable)
      {
        food1.disable = true;
        deleteFood(food1.x, food1.y, food1.size);
      }
    }
      
    if(foodIntakeCheck2()) 
    { // food 2
      if(!food2.disable)
      {
        food2.disable = true;
        deleteFood(food2.x, food2.y, food2.size);
      }
    }

    if(foodIntakeCheck3()) 
    { // food 3
      if(!food3.disable)
      {
        food3.disable = true;
        deleteFood(food3.x, food3.y, food3.size);
      }
    }
    
    if(foodIntakeCheck4()) 
    { // food 4
      if(!food4.disable)
      {
        food4.disable = true;
        deleteFood(food4.x, food4.y, food4.size);
      }
    }

    if (score == quantityFood)
    {
      screenGameCompleted();
      soundGameCompleted();
      levelUp();
      endGame();
    }

    if (checkWalls() || monsterCheck1() || monsterCheck2())
    {
      screenEndGame();
      soundGameOver();
      levelReset();
      endGame();
    }
	
#if DEBUG
    STRING_NUM_L(y_PacMan, 3, 120, 210, getWhite(), getBlack());
    STRING_NUM_L(x_PacMan, 3, 195, 210, getWhite(), getBlack());
#else

    if (score != oldScore)
    {
      oldScore = score;
      scoreUpdate(score);   // Обновляем при изменении
    }
#endif

    pollingButton();

    PT_YIELD(pt);
  }

  PT_END(pt);
}

void runGameEngineThread_pt(void)
{
  GameEngineThread(&gameEngine_pt);
}


