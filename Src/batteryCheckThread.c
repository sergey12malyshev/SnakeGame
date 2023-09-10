#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "SPI_TFT.h"
#include "hard.h"
#include "Screens.h"
#include "batteryCheckThread.h"
#include "colors.h"
#include "menu.h"

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"

static struct pt batteryCheck_pt;

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
    if(!getMenuState())
    {
      STRING_NUM_L(getBatChargePrecent(voltage + getForvardDiodVoltage()), 3, 210, 210, getGreen(), getBlack()); // Выведем заряд
    } 
  }
}

/*
 * Протопоток BatteryCheckThread
 *
 * 
 */
static PT_THREAD(BatteryCheckThread(struct pt *pt))
{
  static uint32_t timeCount = 0;

  PT_BEGIN(pt);

  while (1)
  {
    PT_WAIT_UNTIL(pt, (HAL_GetTick() - timeCount) > 1500U); // Контролируем АКБ ~ раз в 1.5 секунды
    timeCount = HAL_GetTick();	
    
    ADC_conversionRun();
    batteryControlProcess(); 
    heartBeatLedToggle();

    PT_YIELD(pt);
  }

  PT_END(pt);
}

void runBatteryCheckThread_pt(void)
{
  BatteryCheckThread(&batteryCheck_pt);
}


