#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "gameEngineThread.h"
#include "monitorThread.h"
#include "workState.h"
#include "main.h"
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

#define NO_WALS_DEATH      false

#define X_MIN 1U
#define X_MAX 319U
#define Y_MIN 0U
#define Y_MAX 198U

#define LEVEL_MAX 3U
#define WALS_SIZE 5U

typedef uint16_t pixel_t;

typedef enum
{
  NONE = 0,
  UP,
  LEFT,
  DOWN,
  RIGHT
} SPACE_ENUM;

SPACE_ENUM space = UP;

typedef struct
{
  pixel_t x;
  pixel_t y;
} coordinate;

static coordinate pacman = {0};
static coordinate old_pacman = {0};

const pixel_t sizePacMan = 2;
const pixel_t borderPacman = 6; // sizePacMan * 8BIT/2

static int8_t changeX = 0; // changes the direction of the PacMan
static int8_t changeY = -1;

static uint8_t level = 0;

static int8_t score, mainScore, oldMainScore;

/* Параметры еды: */
typedef struct
{
  pixel_t x;
  pixel_t y;
  int8_t size;
  bool disable;
} food;

food food1 = {0}; 
food food2 = {0};
food food3 = {0};
food food4 = {0};

const uint8_t quantityFood = 4;

/* Параметры стен: */
typedef struct
{
  pixel_t x1;
  pixel_t y1;
  pixel_t x2;
  pixel_t y2;
  uint8_t size;
} wals;

wals wals1 = {0}; 
wals wals2 = {0};
wals wals3 = {0};
wals wals4 = {0};

/* Параметры монстров: */
typedef struct
{
  uint8_t type;
  pixel_t x;
  pixel_t y;
  int8_t size;
  uint16_t color;
} monster;

monster monster1 = {0}; 
monster monster2 = {0};


static bool checkWalls(void)
{
  bool rc = false;

  rc |= (((pacman.x + borderPacman >= (wals1.x1)) && (pacman.x - borderPacman <= (wals1.x2 + wals1.size))) && ((pacman.y - borderPacman <= (wals1.y1)) && (pacman.y + borderPacman >= (wals1.y2))));
  rc |= (((pacman.x + borderPacman >= (wals2.x1)) && (pacman.x - borderPacman <= (wals2.x2 + wals2.size))) && ((pacman.y - borderPacman <= (wals2.y1)) && (pacman.y + borderPacman >= (wals2.y2))));
  rc |= (((pacman.x + borderPacman >= (wals3.x1)) && (pacman.x - borderPacman <= (wals3.x2 + wals3.size))) && ((pacman.y - borderPacman <= (wals3.y1)) && (pacman.y + borderPacman >= (wals3.y2))));
  rc |= (((pacman.x + borderPacman >= (wals4.x1)) && (pacman.x - borderPacman <= (wals4.x2 + wals4.size))) && ((pacman.y - borderPacman <= (wals4.y1)) && (pacman.y + borderPacman >= (wals4.y2))));

  return rc;
}

void scoreIncrement(void)
{
  score += 1;
  mainScore += 1;
  beep(10);
}

static inline void up(void)
{
  changeX = 0; // changes the direction of the PacMan
  changeY = -1*sizePacMan;
}

static inline void down(void)
{
  changeX = 0;
  changeY = 1*sizePacMan;
}

static inline void left(void)
{
  changeX = -1*sizePacMan;
  changeY = 0;
}

static inline void right(void)
{
  changeX = 1*sizePacMan;
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

static void levelZero(void)
{
  food1 = (food){25, 45, 6, false}; //Составной литерал C99 http://zonakoda.ru/sostavnye-literaly-v-c99.html
  food2 = (food){240, 10, 6, false};
  food3 = (food){90, 105, 12, false};
  food4 = (food){280, 135, 8, false};

  wals1 = (wals){70, 40, 250, 40, WALS_SIZE}; 
  wals2 = (wals){70, Y_MAX - 40, 250, Y_MAX - 40, WALS_SIZE}; 
  wals3 = (wals){250, Y_MAX - 40, 250, 118, WALS_SIZE};
  wals4 = (wals){250, 80, 250, 40, WALS_SIZE};

  monster1 = (monster){0, 30, 95, 4, COLOR(150, 15, 130)};
  monster2 = (monster){0, 270, 40, 3, COLOR(250, 0, 0)};

  /* Отрисуем еду */
  createFood(food1.x, food1.y, food1.size);
  createFood(food2.x, food2.y, food2.size);
  createFood(food3.x, food3.y, food3.size);
  createFood(food4.x, food4.y, food4.size);

  /* Отрисуем препятствия */
  createWalls(wals1.x1, wals1.y1, wals1.x2, wals1.y2, wals1.size);
  createWalls(wals2.x1, wals2.y1, wals2.x2, wals2.y2, wals1.size);
  createWalls(wals3.x1, wals3.y1, wals3.x2, wals3.y2, wals1.size);
  createWalls(wals4.x1, wals4.y1, wals4.x2, wals4.y2, wals1.size); 

  /* Отрисуем монстов */
  createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
  createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
}

static void levelOne(void)
{
  food1 = (food){50, 100, 11, false}; 
  food2 = (food){280, 25, 8, false};
  food3 = (food){125, 175, 9, false};
  food4 = (food){235, 180, 12, false};

  wals1 = (wals){80, 175, 80, 25, WALS_SIZE}; 
  wals2 = (wals){165, Y_MAX - 2, 165, 115, WALS_SIZE};
  wals3 = (wals){165, 85, 165, Y_MIN, WALS_SIZE};
  wals4 = (wals){250, 90, 250, Y_MIN, WALS_SIZE};

  monster1 = (monster){0, 50, 50, 3, COLOR(255, 0, 0)};
  monster2 = (monster){0, 210, 30, 3, COLOR(0, 0, 255)};

  createFood(food1.x, food1.y, food1.size);
  createFood(food2.x, food2.y, food2.size);
  createFood(food3.x, food3.y, food3.size);
  createFood(food4.x, food4.y, food4.size);

  createWalls(wals1.x1, wals1.y1, wals1.x2, wals1.y2, wals1.size);
  createWalls(wals2.x1, wals2.y1, wals2.x2, wals2.y2, wals2.size);
  createWalls(wals3.x1, wals3.y1, wals3.x2, wals3.y2, wals3.size);
  createWalls(wals4.x1, wals4.y1, wals4.x2, wals4.y2, wals4.size); 

  createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
  createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
}

static void levelTwo(void)
{
  food1 = (food){20, 100, 6, false}; 
  food2 = (food){200, 10, 6, false};
  food3 = (food){100, 160, 12, false};
  food4 = (food){280, 135, 8, false};

  wals1 = (wals){40, 175, 40, 25, WALS_SIZE}; 
  wals2 = (wals){150, Y_MAX - 2, 150, 110, WALS_SIZE};
  wals3 = (wals){185, 120, 185, 5, WALS_SIZE};
  wals4 = (wals){255, 160, 255, 40, WALS_SIZE};

  monster1 = (monster){0, 50, 15, 3, COLOR(55, 55, 255)};
  monster2 = (monster){0, 210, 165, 3, COLOR(222, 0, 230)};

  createFood(food1.x, food1.y, food1.size);
  createFood(food2.x, food2.y, food2.size);
  createFood(food3.x, food3.y, food3.size);
  createFood(food4.x, food4.y, food4.size);

  createWalls(wals1.x1, wals1.y1, wals1.x2, wals1.y2, wals1.size);
  createWalls(wals2.x1, wals2.y1, wals2.x2, wals2.y2, wals2.size);
  createWalls(wals3.x1, wals3.y1, wals3.x2, wals3.y2, wals3.size);
  createWalls(wals4.x1, wals4.y1, wals4.x2, wals4.y2, wals4.size); 

  createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
  createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
}

static void levelThree(void)
{
  food1 = (food){20, 100, 6, false}; 
  food2 = (food){240, 10, 6, false};
  food3 = (food){90, 160, 12, false};
  food4 = (food){280, 135, 8, false};

  wals1 = (wals){110, 178, 110, 0, WALS_SIZE}; 
  wals2 = (wals){140, Y_MAX - 2, 140, 20, WALS_SIZE};
  wals3 = (wals){170, 178, 170, 0, WALS_SIZE};
  wals4 = (wals){200, Y_MAX - 2, 200, 20, WALS_SIZE};

  monster1 = (monster){0, 35, 100, 5, COLOR(23, 150, 108)};
  monster2 = (monster){0, 235, 45, 3, COLOR(0, 0, 255)};

  createFood(food1.x, food1.y, food1.size);
  createFood(food2.x, food2.y, food2.size);
  createFood(food3.x, food3.y, food3.size);
  createFood(food4.x, food4.y, food4.size);

  createWalls(wals1.x1, wals1.y1, wals1.x2, wals1.y2, wals1.size);
  createWalls(wals2.x1, wals2.y1, wals2.x2, wals2.y2, wals1.size);
  createWalls(wals3.x1, wals3.y1, wals3.x2, wals3.y2, wals1.size);
  createWalls(wals4.x1, wals4.y1, wals4.x2, wals4.y2, wals1.size); 

  createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
  createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
}

void initGame(void)
{
  /* Отрисуем рабочее поле */
  createWorkRegion();
  /* Предустановим переменные */
  space = UP;
  up();
  score = 0;
  scoreUpdate(mainScore);
  pacman.x = 215;
  pacman.y = 145;
  old_pacman.x = 0;
  old_pacman.y = 0;
  food1.disable = false;
  food2.disable = false;
  food3.disable = false;
  food4.disable = false;

  switch (level)
  {
    case 0:
      levelZero();
      break;
    case 1:
      levelOne();
      break;
    case 2:
      levelTwo();
      break;
    case LEVEL_MAX:
      levelThree(); 
      break;
    default:
    assert_param(0); // Error
      break;
  }
}

static void endGame(void)
{
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
      initGame();
      break;
    }
  }
}

void levelSet(uint8_t l)
{
  level = l;
}

void levelReset(void)
{
  mainScore = oldMainScore = 0;
  levelSet(0);
}

void levelUp(void)
{
  level++;
  if(level > LEVEL_MAX)
  {
    levelSet(0);
  }
}

static bool foodIntakeCheck1(void)
{
  bool condition1 = (pacman.x <= food1.x + food1.size);
  bool condition2 = (pacman.x >= food1.x - food1.size);
  bool condition3 = (pacman.y <= food1.y + food1.size);
  bool condition4 = (pacman.y >= food1.y - food1.size);

  return condition1 && condition2 && condition3 && condition4;
}

static bool foodIntakeCheck2(void)
{
  bool condition1 = (pacman.x <= food2.x + food2.size);
  bool condition2 = (pacman.x >= food2.x - food2.size);
  bool condition3 = (pacman.y <= food2.y + food2.size);
  bool condition4 = (pacman.y >= food2.y - food2.size);

  return condition1 && condition2 && condition3 && condition4;
}

static bool foodIntakeCheck3(void)
{
  bool condition1 = (pacman.x <= food3.x + food3.size);
  bool condition2 = (pacman.x >= food3.x - food3.size);
  bool condition3 = (pacman.y <= food3.y + food3.size);
  bool condition4 = (pacman.y >= food3.y - food3.size);

  return condition1 && condition2 && condition3 && condition4;
}

static bool foodIntakeCheck4(void)
{
  bool condition1 = (pacman.x <= food4.x + food4.size);
  bool condition2 = (pacman.x >= food4.x - food4.size);
  bool condition3 = (pacman.y <= food4.y + food4.size);
  bool condition4 = (pacman.y >= food4.y - food4.size);

  return condition1 && condition2 && condition3 && condition4;
}  

static bool monsterCheck1(void)
{
  uint8_t realSize = monster1.size*8U;

  return (((pacman.x <= (monster1.x + realSize)) && (pacman.x >= monster1.x)) \
    && ((pacman.y <= (monster1.y + realSize)) && (pacman.y >= monster1.y)));
}
static bool monsterCheck2(void)
{
  uint8_t realSize = monster2.size*8U;

  return (((pacman.x <= (monster2.x + realSize)) && (pacman.x >= monster2.x)) \
    && ((pacman.y <= (monster2.y + realSize)) && (pacman.y >= monster2.y)));
}

static void PacManUpdateProcess(void)
{
  if (((pacman.x <= (old_pacman.x - sizePacMan)) || (pacman.x >= (old_pacman.x + sizePacMan))) \
    || ((pacman.y <= (old_pacman.y - sizePacMan)) || (pacman.y >= (old_pacman.y + sizePacMan))))
  { // update PacMan
#if 0
    fillCircle(old_pacman.x, old_pacman.y, 2, getBlack());
    fillCircle(pacman.x, pacman.y, 2, getYellow());
#else
    disablePacman(old_pacman.x, old_pacman.y);
    createPacman(pacman.x, pacman.y, changeX, changeY);
#endif
  }
} 

__attribute__((unused))static void debugStatus(void)
{
  DEBUG_PRINT("Xpac:%d\r\n", pacman.x);
  DEBUG_PRINT("Ypac:%d\r\n", pacman.y);
  DEBUG_PRINT("Xm2:%d\r\n", monster2.x);
  DEBUG_PRINT("Ym2:%d\r\n", monster2.y);

  if (monsterCheck1())
  {
    DEBUG_PRINT("M1!");
  }
  if (monsterCheck2())
  {
    DEBUG_PRINT("M2!");
  }
  if (checkWalls())
  {
    DEBUG_PRINT("Wals!");
  }

  fillCircle(pacman.x, pacman.y, 2, getRed());
  while(!buttonRightHandler()) WDT_CLEAR;
}

/*
 * Протопоток gameEngineThread. Pac-man Game
 *
 * 
 */
PT_THREAD(GameEngineThread(struct pt *pt))
{
  static uint32_t timerCountGameEngine;
  static uint8_t i;

  PT_BEGIN(pt);
  
  setTime(&timerCountGameEngine);

  while (1)
  {
    PT_DELAY_MS(pt, &timerCountGameEngine, (50u - getSpeedGame()));	

    old_pacman.x = pacman.x;
    old_pacman.y = pacman.y;

    pacman.y = pacman.y + changeY;
    pacman.x = pacman.x + changeX;

    // Захардкодили перемещение монстров
    i++;
    if (i == 70U)
    {
      disableMonster(monster1.x, monster1.y, monster1.size);
      disableMonster(monster2.x, monster2.y, monster2.size);
      monster1.y += 10U;
      monster2.x += 5U;
      createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
      createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
      i = 0U;
    }
    else if(i == 35U)
    {
      disableMonster(monster1.x, monster1.y, monster1.size);
      disableMonster(monster2.x, monster2.y, monster2.size);
      monster1.y -= 10U;
      monster2.x -= 5U;
      createMonster(monster1.type, monster1.x, monster1.y, monster1.size, monster1.color, getBlack());
      createMonster(monster2.type, monster2.x, monster2.y, monster2.size, getBlack(), monster2.color);
    }

#if NO_WALS_DEATH
    if (pacman.y > Y_MAX)
    {
      pacman.y = Y_MIN;
    }
    if (pacman.x > X_MAX)
    {
      pacman.x = X_MIN;
    }
    if (pacman.y < Y_MIN)
    {
      pacman.y = Y_MAX;
    }
    if (pacman.x < X_MIN)
    {
      pacman.x = X_MAX;
    }
#else
    if ((pacman.y + borderPacman >= Y_MAX)||(pacman.x + borderPacman >= X_MAX) \
      ||(pacman.y - borderPacman <= Y_MIN)||(pacman.x - borderPacman <= X_MIN))
    {
      stringGameOver();
      soundGameOver();
      screenEndGame();
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
  #if DEBUG
      debugStatus();
  #endif
      stringGameOver();
      soundGameOver();
      screenEndGame();
      levelReset();
      endGame();
    }
	
    if (mainScore != oldMainScore)
    {
      oldMainScore = mainScore;
      scoreUpdate(mainScore);   // Обновляем при изменении
    }

    pollingButton();

    PT_YIELD(pt);
  }

  PT_END(pt);
}
