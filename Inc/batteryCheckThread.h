#ifndef __BATTERY_CHECK_H
#define __BATTERY_CHECK_H

void runBatteryCheckThread_pt(void);
uint8_t getBatChargePrecent(uint16_t vbat);

#endif /* __BATTERY_CHECK_H */
