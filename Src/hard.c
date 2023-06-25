#include <stdbool.h>
#include "main.h"
#include "hard.h"

extern ADC_HandleTypeDef hadc1;

/*
Standard operating voltage STM32: 2 - 3.6 V
Standard operating voltage li-ion battery: 3 - 4.2 V
Standard operating voltage ili9341: 2.5 - 3.3V 
Standart forward drop silicon diodes: 0.6 - 0.7 V
----------------------------------------------------
Total: Voltage control STM32: 2.5 V - 3.3V
*/
uint16_t getADCvalueVrefint(void)
{ //internal VDA voltage
  uint16_t adcVal = 0;

	HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
	adcVal = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
  
  return adcVal;
}

uint16_t getBatteryVoltage(void)
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

void beep(uint16_t time)
{
  HAL_GPIO_WritePin(PORTB, GPIO_PIN_13, GPIO_PIN_SET);
  HAL_Delay(time);
}
