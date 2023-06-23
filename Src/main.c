/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 *
 * COPYRIGHT(c) 2017 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */

#include "SPI_TFT.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/*X0*******************
 Y0
 *
 *
 *******************/

#define DEBUG              false
#define NO_WALS_DEATH      false

#define X_MIN 1U
#define X_MAX 319U
#define Y_MIN 1U
#define Y_MAX 200U

#define TIME_UPDATE 15

typedef enum
{
  NONE = 0,
  UP,
  LEFT,
  DOWN,
  RIGHT
} SPACE_ENUM;

SPACE_ENUM space = UP;

int16_t x_snake = 240;
int16_t y_snake = 80;
int16_t old_x = 0;
int16_t old_y = 0;

int8_t changeX = 0; // changes the direction of the snake
int8_t changeY = -1;

int16_t score = 0, oldScore = -1;
uint16_t blue_color = COLOR(56, 214, 255);
uint16_t b_color = COLOR(255, 255, 255);
uint16_t orange_color = COLOR(255, 187, 0);
uint8_t timeCount = 0;

/* Параметры еды: */
const uint8_t quantityFood = 3;

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

// Old
uint16_t adc = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
static void screenSaver(void)
{
  const uint16_t colorBg = COLOR(48, 207, 172);
  LCD_Fill(colorBg);
  STRING_OUT("SNAKE GAME", 100, 180, 5, 0x00FF, colorBg);
}

static void screenEndGame(void)
{
  const uint16_t colorBg = COLOR(242, 65, 98);
  LCD_Fill(colorBg);
  STRING_OUT("GAME OVER", 100, 180, 3, 0x00FF, colorBg);
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

  return rc;
}

static void endGame(void)
{
  HAL_Delay(3000);
  LCD_SendCommand(LCD_SWRESET);
  HAL_Delay(1000);
  HAL_NVIC_SystemReset();
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

  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET) && !flagBut2)
  { // обработчик нажатия
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
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
  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET) && flagBut2)
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


/* Hardware */
/*
Standard operating voltage STM32: 2 - 3.6 V
Standard operating voltage li-ion battery: 3 - 4.2 V
Standard operating voltage ili9341: 2.5 - 3.3V 
Standart forward drop silicon diodes: 0.6 - 0.7 V
----------------------------------------------------
Total: Voltage control STM32: 2.5 V - 3.3V
*/
static uint16_t getADCvalueVrefint(void)
{ //internal VDA voltage
  uint16_t adcVal = 0;

	HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 100);
	adcVal = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
  
  return adcVal;
}

static uint16_t getBatteryVoltage(void)
{
  const uint16_t Vrefint = 1200;
  const uint16_t adcData =  getADCvalueVrefint();
  uint16_t voltage_mV = 0;

  if(adcData > 75) // защита от переполнения voltage_mV
  {
    voltage_mV = (Vrefint * 4095) / adcData; 
  } 
  
  return voltage_mV;
}

static bool overVoltageControl(uint16_t voltage)
{
  const uint16_t V_max = 3300;

  if (voltage > V_max)
  {
    return true;
  }
  return false;
}

static bool underVoltageControl(uint16_t voltage)
{
  const uint16_t V_min = 2500;

  if (voltage < V_min)
  {
    return true;
  }
  return false;
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
    STRING_NUM_L(voltage, 4, 190, 210, blue_color, 0x0000); // Выведем напряжение
  }
}

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
   
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */

  HAL_Delay(50); // Добавим задержку, для исключения дребезга питания
  LCD_Init();
  LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);
  
  HAL_ADCEx_Calibration_Start(&hadc1);
  screenSaver();
  HAL_Delay(950);

  /* Отрисуем рабочее поле */
  LCD_Fill(0x0000);
  line(0, 201, 319, 201, 0xFFFF);
  line(0, 0, 0, 199, 0xFFFF);
  STRING_OUT("Score", 15, 210, 1, orange_color, 0x0000);
  STRING_OUT("mV", 270, 210, 1, blue_color, 0x0000);

  /* Отрисуем еду */
  createFood(food1.x, food1.y, food1.size);
  createFood(food2.x, food2.y, food2.size);
  createFood(food3.x, food3.y, food3.size);

  /* Отрисуем препятствия */
  createWalls(wals1.x1, wals1.y1, wals1.x2, wals1.y2);
  createWalls(wals2.x1, wals2.y1, wals2.x2, wals2.y2);
  createWalls(wals3.x1, wals3.y1, wals3.x2, wals3.y2);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  up();
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
    LCD_DrawPixel(x_snake, y_snake, 0XFFFF);
    LCD_DrawPixel(old_x, old_y, 0X0000);


    if (((x_snake <= (food1.x + food1.size)) && (x_snake >= (food1.x - food1.size))) && ((y_snake <= (food1.y + food1.size)) && (y_snake >= (food1.y - food1.size))))
    { // food 1
      if(!food1.disable)
      {
        food1.disable = true;
        score += 1;
        deleteFood(food1.x, food1.y, food1.size);
      }
    }

    if (((x_snake <= (food2.x + food2.size)) && (x_snake >= (food2.x - food2.size))) && ((y_snake <= (food2.y + food2.size)) && (y_snake >= (food2.y - food2.size))))
    { // food 2
      if(!food2.disable)
      {
        food2.disable = true;
        score += 1;
        deleteFood(food2.x, food2.y, food2.size);
      }
    }

    if (((x_snake <= (food3.x + food3.size)) && (x_snake >= (food3.x - food3.size))) && ((y_snake <= (food3.y + food3.size)) && (y_snake >= (food3.y - food3.size))))
    { // food 3
      if(!food3.disable)
      {
        food3.disable = true;
        score += 1;
        deleteFood(food3.x, food3.y, food3.size);
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

/** System Clock Configuration
 */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /**Initializes the CPU, AHB and APB busses clocks
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

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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

  /**Enables the Clock Security System
   */
  HAL_RCC_EnableCSS();

  /**Configure the Systick interrupt time
   */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  /**Configure the Systick
   */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

  /**Common config
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

  /**Configure Regular Channel
   */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

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
}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 4499;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 15;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

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

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
  }
  /* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
