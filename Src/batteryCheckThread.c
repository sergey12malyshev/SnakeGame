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

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"

static struct pt batteryCheck_pt;

const uint16_t green_color = COLOR(17, 255, 0);

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
    STRING_NUM_L(getBatChargePrecent(voltage + getForvardDiodVoltage()), 3, 210, 210, green_color, 0x0000); // Выведем заряд
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

    if((HAL_GetTick() - timeCount) > 1500) // Контролируем АКБ ~ раз в 1.5 секунды
    {
      timeCount = HAL_GetTick();
      batteryControlProcess(); 
      heartBeatLedToggle();
    } 

    PT_YIELD(pt);
  }

  PT_END(pt);
}

void runBatteryCheckThread_pt(void)
{
  BatteryCheckThread(&batteryCheck_pt);
}


