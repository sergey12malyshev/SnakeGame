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

void beep(uint16_t time)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  HAL_Delay(time);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

/* 
Convert vbat [mV] to battery indicator
https://lygte-info.dk/info/BatteryChargePercent%20UK.html

Смотри: SnakeGame\_pdf\SOC_vs_Voltage.xlsx
*/
uint8_t getBatChargePrecent(uint16_t vbat)
{ 
  float charge = 0.1169f*vbat - 385.54f; // y = 0,1169x - 385,54

  if (charge < 0.0) charge = 0.0;
  if (charge > 100.0) charge = 100.0;

  return (uint8_t)charge;
}
