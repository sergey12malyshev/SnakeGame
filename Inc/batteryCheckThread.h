#ifndef __BATTERY_CHECK_H
#define __BATTERY_CHECK_H

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"
PT_THREAD(BatteryCheckThread(struct pt *pt));

uint8_t getBatChargePrecent(uint16_t vbat);

#endif /* __BATTERY_CHECK_H */
