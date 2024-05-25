#ifndef __MONITOR_H
#define __MONITOR_H

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"

#define NEWLINE_STR    "\r\n"

PT_THREAD(MonitorTread(struct pt *pt));

void sendUART(const char *serial_data, ...);
void sendUART_hello(void);
void UART_receve_IT(void);
void resetTest(void);

#endif /* __MONITOR_H */
