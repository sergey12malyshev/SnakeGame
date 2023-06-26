/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "SPI_TFT.h"
#include "hard.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBUG              false
#define NO_WALS_DEATH      false

#define X_MIN 1U
#define X_MAX 319U
#define Y_MIN 0U
#define Y_MAX 198U

#define TIME_UPDATE 10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

const int16_t SWversionMajor = 0;
const int16_t SWversionMinor = 1;

/*X0*******************
 Y0
 *
 *
 *******************/

typedef enum
{
  NONE = 0,
  UP,
  LEFT,
  DOWN,
  RIGHT
} SPACE_ENUM;

SPACE_ENUM space = UP;

const uint16_t sizeSnake = 2;
const uint16_t colorSnake = COLOR(255, 255, 0);
int16_t x_snake, y_snake;
int16_t old_x = 0;
int16_t old_y = 0;

int8_t changeX = 0; // changes the direction of the snake
int8_t changeY = -1;

int16_t score = 0, oldScore = 0;
uint16_t green_color = COLOR(17, 255, 0);
uint16_t b_color = COLOR(255, 255, 255);
uint16_t orange_color = COLOR(255, 187, 0);
uint8_t timeCount = 0;

/* Параметры еды: */
const uint8_t quantityFood = 4;

typedef struct
{
  const int16_t x;
  const int16_t y;
  const int8_t size;
  bool disable;
} food;

food food1 = {50, 100, 11, false}; 
food food2 = {280, 25, 8, false};
food food3 = {125, 175, 9, false};
food food4 = {235, 180, 12, false};

/* Параметры стен: */
typedef struct
{
  uint8_t x1;
  uint8_t y1;
  uint8_t x2;
  uint8_t y2;
} wals;

const wals wals1 = {80, 180, 80, 20}; 
const wals wals2 = {165, Y_MAX, 165, 110};
const wals wals3 = {165, 90, 165, Y_MIN};
const wals wals4 = {250, 90, 250, Y_MIN};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void screenSaver(void)
{
  const uint16_t colorBg = COLOR(48, 207, 172);
  LCD_Fill(colorBg);
  STRING_OUT("SNAKE GAME", 65, 100, 7, 0x00FF, colorBg);
  STRING_OUT("Ver.", 100, 220, 5, 0x00FF, colorBg);
  STRING_NUM_L(SWversionMajor, 1, 180, 220, 0x00FF, colorBg);
  STRING_OUT(".", 195, 220, 4, 0x00FF, colorBg);
  STRING_NUM_L(SWversionMinor, 1, 205, 220, 0x00FF, colorBg); 
}

static void screenEndGame(void)
{
  const uint16_t colorBg = COLOR(242, 65, 98);
  LCD_Fill(colorBg);
  STRING_OUT("GAME OVER", 85, 100, 3, 0x00FF, colorBg);
  STRING_OUT("press button >", 5, 210, 1, 0x00FF, green_color);
}

static void screenGameCompleted(void)
{
  const uint16_t colorBg = COLOR(43, 217, 46);
  LCD_Fill(colorBg);
  STRING_OUT("Good game!", 100, 180, 3, 0x00FF, colorBg);
}

static void screenOverVoltageError(void)
{
  const uint16_t colorBg = COLOR(255, 0, 0);
  LCD_Fill(colorBg);
  STRING_OUT("OVERVOLTAGE!", 80, 180, 3, 0xFFFF, colorBg);
}

static void screenUnderVoltageError(void)
{
  const uint16_t colorBg = COLOR(255, 0, 0);
  LCD_Fill(colorBg);
  STRING_OUT("UNDERVOLTAGE!", 80, 180, 3, 0xFFFF, colorBg);
}

static void createFood(uint16_t x0, uint16_t y0, const uint16_t sizeFood)
{
  const uint16_t green = COLOR(0, 255, 0);
  fillCircle(x0, y0, sizeFood, green);
}

static void deleteFood(uint16_t x0, uint16_t y0, const uint16_t sizeFood)
{
  const uint16_t black = COLOR(0, 0, 0);
  fillCircle(x0, y0, sizeFood, black);
  score += 1;
  beep(10);
}

static void createWalls(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  const uint16_t size = 4;
  const uint16_t blue = COLOR(42, 165, 184);
  H_line(x0, y0, x1, y1, size, blue);
}

static bool checkWalls(void)
{
  bool rc = false;

  rc |= (((x_snake <= (wals1.x1)) && (x_snake >= (wals1.x2))) && ((y_snake <= (wals1.y1)) && (y_snake >= (wals1.y2))));
  rc |= (((x_snake <= (wals2.x1)) && (x_snake >= (wals2.x2))) && ((y_snake <= (wals2.y1)) && (y_snake >= (wals2.y2))));
  rc |= (((x_snake <= (wals3.x1)) && (x_snake >= (wals3.x2))) && ((y_snake <= (wals3.y1)) && (y_snake >= (wals3.y2))));
  rc |= (((x_snake <= (wals4.x1)) && (x_snake >= (wals4.x2))) && ((y_snake <= (wals4.y1)) && (y_snake >= (wals4.y2))));

  return rc;
}

void up()
{
  changeX = 0; // changes the direction of the snake
  changeY = -1;
}

void down()
{
  changeX = 0;
  changeY = 1;
}

void left()
{
  changeX = -1;
  changeY = 0;
}

void right()
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

static void buttonRightHandler(void)
{
  static bool flagBut2 = false;

  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET) && !flagBut2)
  { // обработчик нажатия
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET)
    {
      flagBut2 = true;
      space--;
      if (space == 0)
      {
        space = RIGHT;
      }
      direction();
    }
  }
  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET) && flagBut2)
  { // обработчик отпускания
    flagBut2 = false;
  }
}

static void buttonLeftHandler(void)
{
  static bool flagBut1 = false;

  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) && !flagBut1)
  { // обработчик нажатия
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET)
    {
      flagBut1 = true;
      space++;
      if (space >= 5)
      {
        space = UP;
      }
      direction();
    }
  }
  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET) && flagBut1)
  { // обработчик отпускания
    flagBut1 = false;
  }
}

static void batteryControlProcess(void)
{
  uint16_t voltage = getBatteryVoltage();

  if(overVoltageControl(voltage))
  {
    screenOverVoltageError();
    while (true);
  }
  else if(underVoltageControl(voltage))
  {
    screenUnderVoltageError();
    while (true);
  }
  else
  {
    STRING_NUM_L(voltage, 4, 190, 210, green_color, 0x0000); // Выведем напряжение
  }
}

static void initGame(void)
{
  /* Отрисуем рабочее поле */
  LCD_Fill(0x0000);
  line(0, 201, 319, 201, 0xFFFF);
  line(0, 0, 0, 199, 0xFFFF);
  STRING_OUT("Score", 15, 210, 1, orange_color, 0x0000);
  STRING_NUM_L(score, 2, 125, 210,  orange_color, 0x0000);
  STRING_OUT("mV", 270, 210, 1, green_color, 0x0000);

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
  
  /* Предустановим переменные */
  up();
  oldScore = score = 0;
  x_snake = 215;
  y_snake = 80;
  old_x = 0;
  old_y = 0;
  food1.disable = false;
  food2.disable = false;
  food3.disable = false;
  food4.disable = false;
}

static void endGame(void)
{
  beep(80);
  HAL_Delay(300);
  while ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET));
  initGame();
#if 0
  HAL_Delay(3000);
  LCD_SendCommand(LCD_SWRESET);
  HAL_Delay(1000);
  HAL_NVIC_SystemReset();
#endif
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
   
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  HAL_Delay(50); // Добавим задержку, для исключения дребезга питания
  LCD_Init();
  LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
  
  HAL_ADCEx_Calibration_Start(&hadc1);
  screenSaver();
  HAL_Delay(1100);

  initGame();
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    old_x = x_snake;
    old_y = y_snake;

    y_snake = y_snake + changeY;
    x_snake = x_snake + changeX;

#if NO_WALS_DEATH
    if (y_snake > Y_MAX)
    {
      y_snake = Y_MIN;
    }
    if (x_snake > X_MAX)
    {
      x_snake = X_MIN;
    }
    if (y_snake < Y_MIN)
    {
      y_snake = Y_MAX;
    }
    if (x_snake < X_MIN)
    {
      x_snake = X_MAX;
    }
#else
    if ((y_snake > Y_MAX)||(x_snake > X_MAX)||(y_snake < Y_MIN)||(x_snake < X_MIN))
    {
      screenEndGame();
      endGame();
    }
#endif

    if (((x_snake <= (old_x + sizeSnake)) || (x_snake >= (old_x - sizeSnake))) && ((y_snake <= (old_y + sizeSnake)) && (y_snake >= (old_y - sizeSnake))))
    {
      fillCircle(old_x, old_y, 2, 0X0000);
      fillCircle(x_snake, y_snake, 2, colorSnake);
    }

    if (((x_snake <= (food1.x + food1.size)) && (x_snake >= (food1.x - food1.size))) && ((y_snake <= (food1.y + food1.size)) && (y_snake >= (food1.y - food1.size))))
    { // food 1
      if(!food1.disable)
      {
        food1.disable = true;
        deleteFood(food1.x, food1.y, food1.size);
      }
    }

    if (((x_snake <= (food2.x + food2.size)) && (x_snake >= (food2.x - food2.size))) && ((y_snake <= (food2.y + food2.size)) && (y_snake >= (food2.y - food2.size))))
    { // food 2
      if(!food2.disable)
      {
        food2.disable = true;
        deleteFood(food2.x, food2.y, food2.size);
      }
    }

    if (((x_snake <= (food3.x + food3.size)) && (x_snake >= (food3.x - food3.size))) && ((y_snake <= (food3.y + food3.size)) && (y_snake >= (food3.y - food3.size))))
    { // food 3
      if(!food3.disable)
      {
        food3.disable = true;
        deleteFood(food3.x, food3.y, food3.size);
      }
    }
    
    if (((x_snake <= (food4.x + food4.size)) && (x_snake >= (food4.x - food4.size))) && ((y_snake <= (food4.y + food4.size)) && (y_snake >= (food4.y - food4.size))))
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
      endGame();
    }

    if (checkWalls())
    {
      screenEndGame();
      endGame();
    }

    buttonLeftHandler();
    buttonRightHandler();
	
#if DEBUG
    STRING_NUM_L(y_snake, 3, 120, 210, b_color, 0x0000);
    STRING_NUM_L(x_snake, 3, 195, 210, b_color, 0x0000);
#else
    if(++timeCount > 66 * (15 / TIME_UPDATE)) // При уменьшении TIME_UPDATE задержка в 1 с сохранится!
    {
      timeCount = 0;
      batteryControlProcess(); // Контролируем АКБ ~ раз в секунду
    } 

    if (score != oldScore)
    {
      oldScore = score;
      STRING_NUM_L(score, 2, 125, 210,  orange_color, 0x0000);   // Обновляем при изменении
    }
#endif

    HAL_Delay(TIME_UPDATE); // (развертка по Х)
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
