#ifndef __HARD_H
#define __HARD_H

uint16_t getADCvalueVrefint(void);
uint16_t getBatteryVoltage(void);
uint16_t getForvardDiodVoltage(void);
bool overVoltageControl(uint16_t voltage);
bool underVoltageControl(uint16_t voltage);
void beep(uint16_t time);
uint8_t getBatChargePrecent(uint16_t vbat);
void heartBeatLedEnable(void);
void heartBeatLedToggle(void);
#endif /* __HARD_H */
