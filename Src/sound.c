#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "hard.h"


void beep(uint16_t time)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  HAL_Delay(time);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

void soundGameOver(void)
{
  for(uint8_t i = 0; i < 60; i++)
  {
    beep(1);
    HAL_Delay(2);
  }
}

void soundGameCompleted(void)
{
  for(uint8_t i = 0; i < 10; i++)
  {
    beep(40);
    HAL_Delay(2);
  }
}

void soundPowerOn(void)
{
  soundGameCompleted();
}

