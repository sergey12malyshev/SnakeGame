#ifndef __MONITOR_H
#define __MONITOR_H

void runMonitorTread_pt(void);
void sendUART(const uint8_t* TxBufferUartLocal);
void sendUART_hello(void);
void UART_receve_IT(void);
void resetTest(void);

#endif /* __MONITOR_H */
