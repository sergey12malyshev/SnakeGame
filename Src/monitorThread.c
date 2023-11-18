#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "hard.h"
#include "monitorThread.h"
#include "batteryCheckThread.h"
#include "gameEngineThread.h"
#include "Sound.h"

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


static const uint8_t mon_comand[] = "Enter monitor command:\r\n\
HELP - see existing commands\r\n\
RST - restart\r\n\
STOP - stop game process\r\n\
TEST - run sound test\r\n\
ADC - show ADC chanel bat\r\n\
BAT - show bat voltage (0.01V) and stat. charge\r\n\
LV2 - set level 2\r\n\
LV3 - set level 3\r\n\
INFO - read about project\r\n\
>";

static uint8_t symbol_term[] = ">";

uint8_t input_mon[1] = {0};
char input_mon_buff[SIZE_BUFF] = {0};

static uint8_t str[50]= {0};

COMAND monitorTest = NONE;

//-------------- UART -------------------//

void clear_uart_buff(void)
{
  memset(input_mon_buff, 0, sizeof(input_mon_buff));
}

void sendUART(const uint8_t* TxBufferUartLocal)
{ //передача в блокирующем режиме
  const uint8_t block_timeout_ms = 40; //t(sec)=(FRAME/BOUND+MINT)*N = (10/115200+0.00001)*100 = 19 мс

  HAL_UART_Transmit(&huart1, (uint8_t *) TxBufferUartLocal, strlen((char *) TxBufferUartLocal), block_timeout_ms);
}

void UART_receve_IT(void)
{
  HAL_UART_Receive_IT(&huart1, (uint8_t *)input_mon, 1);
}

static void sendUART_symbolTerm(void)
{
  sendUART((uint8_t *)symbol_term);
}

static void sendSNversion(void)
{
  extern const int16_t SWversionMajor, SWversionMinor, SWversionPatch;

  sprintf((char *)str, "Version: %d", SWversionMajor);
  sendUART((uint8_t *)str);
  sendUART((uint8_t *)".");
  sprintf((char *)str, "%d", SWversionMinor);
  sendUART((uint8_t *)str);
  sendUART((uint8_t *)".");
  sprintf((char *)str, "%d", SWversionPatch);
  sendUART((uint8_t *)str);
  sendUART((uint8_t *)NEWLINE_STR);
}

void sendUART_hello(void)
{
  static const uint8_t hello_string[] = "Pac-ManGame\r\n";
  static const uint8_t enter_help[] = "Enter HELP\r\n";

  sendUART((uint8_t *)hello_string);
  sendSNversion();
#if DEBUG_MAIN
  sendUART((uint8_t *)"Debug build!\r\n");
#endif
  sendUART((uint8_t *)enter_help);
  sendUART_symbolTerm();
}

void sendUART_help(void)
{
  sendUART((uint8_t *)mon_comand);
}

static void sendUART_OK(void)
{
  static const uint8_t mon_OK[] = "OK\r\n";
  sendUART((uint8_t *)mon_OK);
}

static void sendUART_r_n(void)
{
  static uint8_t r_n[] = "\r\n";
  sendUART((uint8_t *)r_n);
}

static void sendUART_error(void)
{
  static const uint8_t error[] = "incorrect enter\r\n";
  sendUART((uint8_t *)error);
}

static void sendBackspaceStr(void)
{
  static const uint8_t backspace_str[] = " \b";
  sendUART((uint8_t *)backspace_str);
}

static void convertToUppercase(void)
{
  static char *copy_ptr;

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
  const uint8_t Backspace = 0x08;

  if ((huart1.RxXferCount == 0) && (HAL_UART_Receive_IT(&huart1, input_mon, 1) != HAL_BUSY))
  {
#if LOCAL_ECHO_EN
    HAL_UART_Transmit(&huart1, input_mon, 1, 50); // Local echo
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
        sendUART((uint8_t *)"https://github.com/sergey12malyshev/Pac-ManGame\r\n");
        sendUART((uint8_t *)"HAL: ");
        sprintf((char *)str, "%ld", HAL_GetHalVersion());
        sendUART(str);
        sendUART_r_n();
        sendUART((uint8_t *)__DATE__);
        sendUART_r_n();
        sendUART((uint8_t *)__TIME__);
      }
      else if (mon_strcmp(input_mon_buff, "LV3"))
      {
        levelSet(2);
        initGame();
        sendUART_OK();
      }
      else if (mon_strcmp(input_mon_buff, "LV2"))
      {
        levelSet(1);
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
      if (input_mon[0] == Backspace)
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
            sendUART((uint8_t *)"\r\nswitch keyboard language\r\n");
          }
          
        }
        else
        {
          sendUART((uint8_t *)"\r\noverflow\r\n");
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
      sprintf((char *)str, "%d\r\n", getADCvalueVrefint());
      sendUART((uint8_t *)str);
      resetTest();
      break;
    case BAT:
      batVoltFilt = getBatteryVoltageFilter();
      sprintf((char *)str, "System voltage, mV: %d\r\n", getSystemVoltage());
      sendUART((uint8_t *)str);
      sprintf((char *)str, "Battery voltage, mV: %d\r\n", getBatteryVoltage());
      sendUART((uint8_t *)str);
      sprintf((char *)str, "Battery filter voltage, mV: %d\r\n", batVoltFilt);
      sendUART((uint8_t *)str);
      sprintf((char *)str, "Battery charge, %%: %d\r\n", getBatChargePrecent(batVoltFilt));
      sendUART((uint8_t *)str);
      resetTest();
      break;
    case TEST:
      soundGameOver();
      soundGameCompleted();
      sendUART((uint8_t *)"TEST Ok\r\n");
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
