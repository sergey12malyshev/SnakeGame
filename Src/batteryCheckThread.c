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
#include "sound.h"
#include "workState.h"
#include "time.h"

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"

/*
Standard operating voltage STM32: 2 - 3.6 V
Standard operating voltage li-ion battery: 3 - 4.2 V
Standard operating voltage ili9341: 2.5 - 3.3V 
Standart forward drop silicon diodes: 0.6 - 0.7 V
----------------------------------------------------
Total: Voltage control STM32: 2.5 V - 3.3V
*/

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

bool overVoltageControl(uint16_t voltage)
{
  const uint16_t V_max = 3300; //max V ili9341
  bool rc = false;

  if (voltage > V_max)
  {
    rc = true;
  }
  return rc;
}

bool underVoltageControl(uint16_t voltage)
{
  const uint16_t V_min = 2500; //min V ili9341
  bool rc = false;

  if (voltage < V_min)
  {
    rc = true;
  }
  return rc;
}

static void systemControlProcess(void)
{
  uint16_t voltage = getSystemVoltage();

  if(overVoltageControl(voltage))
  {
    screenOverVoltageError();
    beep(1000);
    while (true) WDT_CLEAR;
  }
  else if(underVoltageControl(voltage))
  {
    screenUnderVoltageError();
    beep(1000);
    while (true) WDT_CLEAR;
  }
}

/*
 * Протопоток BatteryCheckThread
 *
 * 
 */
PT_THREAD(BatteryCheckThread(struct pt *pt))
{
  static uint32_t timer1, timeCount;

  PT_BEGIN(pt);
  
  setTime(&timer1);

  while (1)
  {
    PT_DELAY_MS(pt, &timer1, 50U); // Запускаем преобразования ~ раз в 50 мс
    
    ADC_conversionRun();
    batteryVoltageFilterProcess();

    if(++timeCount > 20) // Каждую секунду выводим заряд и контролируем системное напряжние
    {
      timeCount = 0;
      heartBeatLedToggle();
      systemControlProcess(); 
      if(getWorkState() != MENU)
      {
        STRING_NUM_L(getBatChargePrecent(getBatteryVoltageFilter()), 3, 210, 210, getGreen(), getBlack());
      } 
    }

    PT_YIELD(pt);
  }

  PT_END(pt);
}
