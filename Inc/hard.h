#ifndef __HARD_H
#define __HARD_H

uint16_t getADCvalueVrefint(void);
uint16_t getBatteryVoltage(void);
uint16_t getForvardDiodVoltage(void);
bool overVoltageControl(uint16_t voltage);
bool underVoltageControl(uint16_t voltage);
void heartBeatLedEnable(void);
void heartBeatLedToggle(void);
#endif /* __HARD_H */
