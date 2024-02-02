#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "main.h"
#include "hard.h"
#include "monitorThread.h"
#include "batteryCheckThread.h"
#include "gameEngineThread.h"
#include "Sound.h"
#include "runBootloader.h"

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"

#define LOCAL_ECHO_EN  1U
#define SIZE_BUFF      12U

#define NEWLINE_STR    "\r\n"
#define mon_strcmp(ptr, cmd) (!strcmp(ptr, cmd))

extern UART_HandleTypeDef huart1;

/*
  UART CLI 115200 Baud
  PA10 - RX
  PA9 - TX
*/

typedef enum
{
  NONE = 0,
  RST,
  R,
  TEST,
  ADC,
  BAT,
  INFO
}COMAND;

static const char mon_comand[] = "Enter monitor command:\r\n\
HELP - see existing commands\r\n\
RST - restart\r\n\
LOAD - run bootloader\r\n\
STOP - stop game process\r\n\
TEST - run sound test\r\n\
ADC - show ADC chanel bat\r\n\
BAT - show bat voltage (0.01V) and stat. charge\r\n\
LV2 - set level 2\r\n\
LV3 - set level 3\r\n\
NEXT - set next level\r\n\
INFO - read about project\r\n\
>";

static char input_mon[1] = {0};
static char input_mon_buff[SIZE_BUFF] = {0};
static char printBufer[350] = {0};

COMAND monitorTest = NONE;

//-------------- UART -------------------//

void sendUART(const char *serial_data, ...)
{
  const uint8_t block_timeout_ms = 40;

  va_list arg;
  va_start(arg, serial_data);
  uint16_t len = vsnprintf(printBufer, sizeof(printBufer), serial_data, arg);
  va_end(arg);

  HAL_UART_Transmit(&huart1, (uint8_t *)printBufer, len, block_timeout_ms);
}

void sendUARTstring(const uint8_t* TxBufferUartLocal)
{ 
  const uint8_t block_timeout_ms = 40; //t(sec)=(FRAME/BOUND+MINT)*N = (10/115200+0.00001)*100 = 19 мс
  HAL_UART_Transmit(&huart1, (uint8_t *) TxBufferUartLocal, strlen((char *) TxBufferUartLocal), block_timeout_ms);
}

static void clear_uart_buff(void)
{
  memset(input_mon_buff, 0, sizeof(input_mon_buff));
}

void UART_receve_IT(void)
{
  HAL_UART_Receive_IT(&huart1, (uint8_t *)input_mon, 1);
}

static void sendUART_symbolTerm(void)
{
  static char symbol_term[] = ">";
  sendUART(symbol_term);
}

static void sendSNversion(void)
{
  extern const int16_t SWversionMajor, SWversionMinor, SWversionPatch;

  sendUART("Version: %d.%d.%d", SWversionMajor, SWversionMinor, SWversionPatch);
  sendUART(NEWLINE_STR);
}

void sendUART_hello(void)
{
  static const char hello_string[] = "GameBox console started!\r\n";
  static const char enter_help[] = "Enter HELP\r\n";

  sendUART(hello_string);
  sendSNversion();
#if DEBUG_MAIN
  sendUART("Debug build!\r\n");
#endif
  sendUART(enter_help);
  sendUART_symbolTerm();
}

void sendUART_help(void)
{
  sendUART(mon_comand);
}

static void sendUART_OK(void)
{
  static const char mon_OK[] = "OK\r\n";
  sendUART(mon_OK);
}

static void sendUART_r_n(void)
{
  static char r_n[] = "\r\n";
  sendUART(r_n);
}

static void sendUART_error(void)
{
  static const char error[] = "incorrect enter\r\n";
  sendUART(error);
}

static void sendBackspaceStr(void)
{
  static const char backspace_str[] = " \b";
  sendUART(backspace_str);
}

static void convertToUppercase(void)
{
  static char *copy_ptr = NULL;

  copy_ptr = input_mon_buff;
  while (*copy_ptr != 0)
  {
    *copy_ptr = toupper(*copy_ptr);
    copy_ptr++;
  }
}

void resetTest(void)
{
  monitorTest = NONE;
}

static void monitor(void)
{
  static uint8_t rec_len = 0U;
  const uint8_t enter = 13U;
  const uint8_t backspace = 0x08;

  if ((huart1.RxXferCount == 0) && (HAL_UART_Receive_IT(&huart1, (uint8_t*)input_mon, 1) != HAL_BUSY))
  {
#if LOCAL_ECHO_EN
    HAL_UART_Transmit(&huart1, (uint8_t*)input_mon, 1, 50); // Local echo
#endif
    if (input_mon[0] == enter)
    {
      convertToUppercase();
      sendUART_r_n();
      if (mon_strcmp(input_mon_buff, "HELP"))
      {
        sendUART_help();
      }
      else if (mon_strcmp(input_mon_buff, "TEST"))
      {
        monitorTest = TEST;
      }
      else if (mon_strcmp(input_mon_buff, "STOP"))
      {
        while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET) WDT_CLEAR;
      }
      else if (mon_strcmp(input_mon_buff, "LOAD"))
      {
        sendUART_OK();
        runBootloader();
      }
       else if (mon_strcmp(input_mon_buff, "ADC"))
      {
        monitorTest = ADC;
        sendUART_OK();
      }
      else if (mon_strcmp(input_mon_buff, "BAT"))
      {
        monitorTest = BAT;
        sendUART_OK();
      }
      else if (mon_strcmp(input_mon_buff, "RST"))
      {
        sendUART_OK();
        HAL_NVIC_SystemReset();
      }
      else if (mon_strcmp(input_mon_buff, "INFO"))
      {
        sendUART_OK();
        sendUART("https://github.com/sergey12malyshev/Pac-ManGame" NEWLINE_STR);
        sendUART("HAL: %ld"NEWLINE_STR, HAL_GetHalVersion());
        sendUART("Data build: "__DATE__ NEWLINE_STR);
        sendUART("Time build: "__TIME__ NEWLINE_STR ">");
      }
      else if (mon_strcmp(input_mon_buff, "LV3"))
      {
        levelReset();
        levelSet(2);
        initGame();
        sendUART_OK();
      }
      else if (mon_strcmp(input_mon_buff, "LV2"))
      {
        levelReset();
        levelSet(1);
        initGame();
        sendUART_OK();
      }
      else if (mon_strcmp(input_mon_buff, "NEXT"))
      {
        levelUp();
        initGame();
        sendUART_OK();
      }
      else
      {
        if (input_mon_buff[0] == 0)
        {
          sendUART_symbolTerm();
          clear_uart_buff();
          rec_len = 0;
          resetTest();
        }
        else
        {
          sendUART_error();
          sendUART_symbolTerm();
        }
      }
      clear_uart_buff();
      rec_len = 0;
    }
    else
    {
      if (input_mon[0] == backspace)
      {
        if (rec_len != 0)
        {
          input_mon_buff[rec_len - 1] = 0;
          rec_len--;
          sendBackspaceStr();
        }
      }
      else
      {
        if (rec_len < SIZE_BUFF)
        {
          if((input_mon[0] > 0) && (input_mon[0] <= 127)) //ASCIi check
          {
            input_mon_buff[rec_len++] = input_mon[0]; //load char do string
          }
          else
          {
            sendUART(NEWLINE_STR"switch keyboard language"NEWLINE_STR);
          }
          
        }
        else
        {
          sendUART(NEWLINE_STR"overflow" NEWLINE_STR);
        }
      }
    }
  }
}

static void monitor_out_test(void)
{
  uint16_t batVoltFilt = 0;

  switch (monitorTest)
  {
    case ADC:
      sendUART("%d\r\n", getADCvalueVrefint());
      resetTest();
      break;
    case BAT:
      batVoltFilt = getBatteryVoltageFilter();
      sendUART("System voltage, mV: %d\r\n", getSystemVoltage());
      sendUART("Battery voltage, mV: %d\r\n", getBatteryVoltage());
      sendUART("Battery filter voltage, mV: %d\r\n", batVoltFilt);
      sendUART("Battery charge, %%: %d\r\n", getBatChargePrecent(batVoltFilt));
      resetTest();
      break;
    case TEST:
      soundGameOver();
      soundGameCompleted();
      sendUART("TEST Ok\r\n");
      resetTest();
      break;
    default:;
  }
}


/*
 * Протопоток MonitorTread
 *
 */
PT_THREAD(MonitorTread(struct pt *pt))
{
  static uint32_t timeCount = 0;

  PT_BEGIN(pt);

  while (1)
  {
    PT_WAIT_UNTIL(pt, (HAL_GetTick() - timeCount) > 50U);
    timeCount = HAL_GetTick();	

    monitor();
    monitor_out_test();

    PT_YIELD(pt);
  }

  PT_END(pt);
}
