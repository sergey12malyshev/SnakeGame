#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "runBootloader.h" 
#include "monitorThread.h"
#include "Screens.h"

/*
* It is impossible to jump the system Bootloader(0x1FFFF000). Because STM32F1 series does not support the memory remap function.
* Невозможно перепрыгнуть системный загрузчик (0x1FFFF000). Поскольку серия STM32F1 не поддерживает функцию перераспределения памяти.
* Для обновления заряжаем RC-цепочку на ножке Boot и вызываем системный сброс. Загрузчик работает через UART1
*/

void runBootloader(void)
{
  HAL_GPIO_WritePin(BOOT_EN_GPIO_Port, BOOT_EN_Pin, GPIO_PIN_SET);
  sendUART(OK_G "Run DFU mode\r\n");
  HAL_Delay(25);
  NVIC_SystemReset();  
}