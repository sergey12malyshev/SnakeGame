#include <stdbool.h>
#include "main.h"
#include "hard.h"
#include "filter.h"

extern ADC_HandleTypeDef hadc1;
static uint16_t adcValue = 0, batVoltageFilt = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance == ADC1) //check if the interrupt comes from ACD1
  {
    adcValue = HAL_ADC_GetValue(&hadc1);
  }
}

void ADC_conversionRun(void)
{
  HAL_ADC_Start_IT(&hadc1);
}

uint16_t getADCvalueVrefint(void)
{ //internal VDA voltage
  return adcValue;
}

uint16_t getSystemVoltage(void)
{
  const uint16_t Vrefint = 1200;
  const uint16_t adcData =  getADCvalueVrefint();
  uint16_t voltage_mV = 0;

  if(adcData > 75U) // защита от переполнения voltage_mV
  {
    voltage_mV = (Vrefint * 4095) / adcData; 
  } 
  
  return voltage_mV;
}

uint16_t getBatteryVoltage(void)
{
  const uint16_t forvard_Diod_mV = 825; // падение на диоде
  const uint16_t forvard_PMOS_mV = 95; // падение на PMOS

  uint16_t voltage_mV = getSystemVoltage() + forvard_Diod_mV + forvard_PMOS_mV;

  return voltage_mV;
}

void batteryVoltageFilterProcess(void)
{
  batVoltageFilt = expRunningAverageFilter(getBatteryVoltage());
}

uint16_t getBatteryVoltageFilter(void)
{
  return batVoltageFilt;
}

void heartBeatLedEnable(void)
{
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void heartBeatLedToggle(void)
{
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}


bool buttonLeftHandler(void)
{
  static bool flagBut1 = false;
  bool rc = false;

  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) && !flagBut1)
  { // обработчик нажатия
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET)
    {
      flagBut1 = true;
      rc = true;
    }
  }
  
  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET) && flagBut1)
  { // обработчик отпускания
    flagBut1 = false;
  }
  
  return rc;
}

bool buttonRightHandler(void)
{
  static bool flagBut1 = false;
  bool rc = false;

  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET) && !flagBut1)
  { // обработчик нажатия
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET)
    {
      flagBut1 = true;
      rc = true;
    }
  }
  
  if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET) && flagBut1)
  { // обработчик отпускания
    flagBut1 = false;
  }
  
  return rc;
}

/* Flash memory API */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x800FC00) /* Base adress of PAGE 63, 1024 byte 
0x400 * 63 (dec) =  FC00 */

uint32_t flash_get_page(void)
{
  return ADDR_FLASH_PAGE_63;
}

uint32_t flash_read(uint32_t address)
{
  return *(uint32_t*)address;
}

#if 1 // 1 = Use HAL, 0 = CMSIS
uint32_t flash_write(uint32_t address, uint32_t data)
{
  uint32_t pageError = 0;
  static FLASH_EraseInitTypeDef EraseInitStruct;

  HAL_FLASH_Unlock();
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = address;
  EraseInitStruct.NbPages = 1;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &pageError) != HAL_OK) //Erase the Page Before a Write Operation
  {
    return HAL_FLASH_GetError();
  }

  HAL_Delay(1);
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, (uint64_t)data) != HAL_OK)
  {
    return HAL_FLASH_GetError();
  }
  
  HAL_Delay(1);
  HAL_FLASH_Lock();

  return 0;
}
#else

uint8_t checkBusyFlash(void)
{
  while(FLASH->SR & (FLASH_SR_BSY))
  {
    __NOP();
  } 

  if (FLASH->CR & FLASH_SR_EOP)
  {
    FLASH->SR = FLASH_SR_EOP;
  }

  if(FLASH->SR & FLASH_SR_WRPRTERR  || \
     FLASH->SR & (OBR_REG_INDEX << 8U | FLASH_OBR_OPTERR) || \
     FLASH->SR & FLASH_SR_PGERR)
  {
    return 1;
  }

  return 0;
}

static inline void flash_unlock(void)
{
  if((FLASH->CR & FLASH_CR_LOCK) != 0)
  {
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }
}

static inline void flash_lock(void)
{
  FLASH->CR |= FLASH_CR_LOCK;
}

uint8_t flash_Erase(const uint32_t pageAddress)
{
  uint8_t rc = 0;

  flash_unlock();

  rc = checkBusyFlash();

  FLASH->CR |= FLASH_CR_PER;
  FLASH->AR = pageAddress;
  FLASH->CR |= FLASH_CR_STRT;
  
  rc = checkBusyFlash();

  FLASH->CR &= ~FLASH_CR_PER;

  flash_lock();

  return rc;
}

uint8_t flash_Programm(uint32_t Address, uint64_t Data)
{
  uint8_t rc = 0;
  const uint8_t nbiterations = 2; //nbiterations FLASH_TYPEPROGRAM_WORD

  flash_unlock();

  rc = checkBusyFlash();

  for (uint8_t index = 0U; index < nbiterations; index++)
  {
    //FLASH_Program_HalfWord((Address + (2U*index)), (uint16_t)(Data >> (16U*index)));
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint16_t*)(Address + (2U*index)) = (uint16_t)(Data >> (16U*index)); /* Write data in the address */
    rc = checkBusyFlash();
    
    FLASH->CR &= ~ FLASH_CR_PG;
  }
  
  flash_lock();

  return rc;
}

uint32_t flash_write(uint32_t address, uint32_t data)
{
  if (flash_Erase(address) != 0) //Erase the Page Before a Write Operation
  {
    return 1;
  }

  HAL_Delay(1);
  if (flash_Programm(address, (uint64_t)data) != 0)
  {
    return 1;
  }
  
  HAL_Delay(1);

  return 0;
}
#endif
