#ifndef __HARD_H
#define __HARD_H

void ADC_conversionRun(void);
uint16_t getADCvalueVrefint(void);
uint16_t getBatteryVoltage(void);
uint16_t getSystemVoltage(void);
void batteryVoltageFilterProcess(void);
uint16_t getBatteryVoltageFilter(void);
void heartBeatLedEnable(void);
void heartBeatLedToggle(void);
bool buttonLeftHandler(void);
bool buttonRightHandler(void);
uint32_t flash_get_page(void);
uint32_t flash_read(uint32_t address);
uint32_t flash_write(uint32_t address, uint32_t data);
void delay_ap(uint32_t delay);


#endif /* __HARD_H */
