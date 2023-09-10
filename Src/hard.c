#include <stdbool.h>
#include "main.h"
#include "hard.h"

extern ADC_HandleTypeDef hadc1;
static uint16_t adcValue = 0;
/*
Standard operating voltage STM32: 2 - 3.6 V
Standard operating voltage li-ion battery: 3 - 4.2 V
Standard operating voltage ili9341: 2.5 - 3.3V 
Standart forward drop silicon diodes: 0.6 - 0.7 V
----------------------------------------------------
Total: Voltage control STM32: 2.5 V - 3.3V
*/

void ADC_conversionRun(void)
{
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 100);
  adcValue = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
}

uint16_t getADCvalueVrefint(void)
{ //internal VDA voltage
  return adcValue;
}

uint16_t getBatteryVoltage(void)
{
  const uint16_t Vrefint = 1200;
  const uint16_t adcData =  getADCvalueVrefint();
  uint16_t voltage_mV = 0;

  if(adcData > 75U) // защита от переполнения voltage_mV
  {
    voltage_mV = (Vrefint * 4095) / adcData; 
  } 
  
  return voltage_mV;
}

uint16_t getForvardDiodVoltage(void)
{
  const uint16_t forvard_Diod_mV = 820; // падение на диоде
  return forvard_Diod_mV;
}

bool overVoltageControl(uint16_t voltage)
{
  const uint16_t V_max = 3300;

  if (voltage > V_max)
  {
    return true;
  }
  return false;
}

bool underVoltageControl(uint16_t voltage)
{
  const uint16_t V_min = 2500;

  if (voltage < V_min)
  {
    return true;
  }
  return false;
}

void heartBeatLedEnable(void)
{
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void heartBeatLedToggle(void)
{
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}


bool buttonLeftHandler(void)
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

bool buttonRightHandler(void)
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
