#ifndef __HARD_H
#define __HARD_H

uint16_t getADCvalueVrefint(void);
uint16_t getBatteryVoltage(void);
bool overVoltageControl(uint16_t voltage);
bool underVoltageControl(uint16_t voltage);
void beep(uint16_t time);
#endif /* __HARD_H */
