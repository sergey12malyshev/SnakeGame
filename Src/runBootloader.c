#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "runBootloader.h" 
#include "Screens.h"

/*
* It is impossible to jump the system Bootloader(0x1FFFF000). Because STM32F1 series does not support the memory remap function.
* Невозможно перепрыгнуть системный загрузчик (0x1FFFF000). Поскольку серия STM32F1 не поддерживает функцию перераспределения памяти.
* Для обновления заряжаем RC-цепочку на ножке Boot и вызываем системный сброс. Загрузчик работает через UART1
*/

void runBootloader(void)
{
  screenBoot();
  HAL_GPIO_WritePin(BOOT_EN_GPIO_Port, BOOT_EN_Pin, GPIO_PIN_SET);
  HAL_Delay(25);
  NVIC_SystemReset();  
}