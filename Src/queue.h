#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdio.h>

#define MESSAGE_SIZE    1U     /* Размер принимаемого собщения - 1 байт для UART */

typedef struct 
{
  uint8_t msg[MESSAGE_SIZE];
} MESSAGE;

void init_queueUart(void);
bool enqueUart(const uint8_t data[]);
bool dequeUart(void);


#endif /* __QUEUE_H */
