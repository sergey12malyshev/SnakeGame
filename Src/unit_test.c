/*
Run Unit Test Moule
*/
#include <stdbool.h>

#include "unit_test.h"
#include "main.h"
#include "batteryCheckThread.h"
#include "monitorThread.h"


static bool Test_getBatChargePrecent(void)
{
  return getBatChargePrecent(3700) == 46u;
}

static bool Test_overVoltageControl(void)
{
  return overVoltageControl(3400);
}

static bool Test_underVoltageControl(void)
{
  return underVoltageControl(2400);
}

static void stopTest(void)
{
  sendUART((uint8_t *)"Unit Tests failed!\r\n");
  for(;;)
  {
    WDT_CLEAR;
  }
}

void runUnitTests(void)
{
  if (Test_getBatChargePrecent() == false)
  {
    sendUART((uint8_t *)"1\r\n");
    stopTest();
  }
  if(Test_overVoltageControl() == false)
  {    
    sendUART((uint8_t *)"2\r\n");
    stopTest();
  }
  if(Test_underVoltageControl() == false)
  {
    sendUART((uint8_t *)"3\r\n");
    stopTest();
  }

  sendUART((uint8_t *)"Unit Tests passed!\r\n");
}
