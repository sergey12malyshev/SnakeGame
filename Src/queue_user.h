#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdio.h>

#define MESSAGE_SIZE    1U     /* Размер принимаемого собщения - 1 байт для UART */
#define QUEUE_SIZE      64     /* Queue size (number of buffer cells) */

typedef struct 
{
  uint8_t msg[MESSAGE_SIZE];
} MESSAGE;

typedef struct 
{
  MESSAGE messages[QUEUE_SIZE];
  int begin;
  int end;
  int current_load;
} QUEUE;

void init_queue(QUEUE *queue);
bool enque(QUEUE *queue, MESSAGE *message);
bool deque(QUEUE *queue, MESSAGE *message);


#endif /* __QUEUE_H */
