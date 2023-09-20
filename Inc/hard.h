#ifndef __HARD_H
#define __HARD_H

void ADC_conversionRun(void);
uint16_t getADCvalueVrefint(void);
uint16_t getBatteryVoltage(void);
uint16_t getSystemVoltage(void);
void heartBeatLedEnable(void);
void heartBeatLedToggle(void);
bool buttonLeftHandler(void);
bool buttonRightHandler(void);
#endif /* __HARD_H */
