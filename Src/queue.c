#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"
#include "monitorThread.h"

/*
Portable array-based cyclic FIFO queue. https://stackoverflow.com/questions/52783068/how-to-implement-a-message-queue-in-standard-c
*/

#define QUEUE_SIZE      64    /* Queue size (number of buffer cells) */

#define DEBUG_QUEUE     0

MESSAGE outBufer = {0};

typedef struct 
{
  MESSAGE messages[QUEUE_SIZE];
  int begin;
  int end;
  int current_load;
} QUEUE;


static void init_queue(QUEUE *queue) 
{
  queue->begin = 0;
  queue->end = 0;
  queue->current_load = 0;
  memset(&queue->messages[0], 0, QUEUE_SIZE * sizeof(MESSAGE));
}

static bool enque(QUEUE *queue, MESSAGE *message) 
{
  if (queue->current_load < QUEUE_SIZE) 
  {
    if (queue->end == QUEUE_SIZE) 
    {
      queue->end = 0;
    }
    queue->messages[queue->end] = *message;
    queue->end++;
    queue->current_load++;

    return true;
  } 
  return false;
}

static bool deque(QUEUE *queue, MESSAGE *message) 
{
  if (queue->current_load > 0) 
  {
    *message = queue->messages[queue->begin];
    memset(&queue->messages[queue->begin], 0, sizeof(MESSAGE));
    queue->begin = (queue->begin + 1) % QUEUE_SIZE;
    queue->current_load--;

    return true;
  } 
  return false;
}

/* queue UART */
QUEUE queue1 = {0};
MESSAGE massage1 = {0};

void init_queueUart(void)
{
  init_queue(&queue1);
}

bool enqueUart(const uint8_t *data) 
{
#if DEBUG_QUEUE
  sendUART("e_ l:%d e:%d b:%d\r\n", queue1.current_load, queue1.begin, queue1.end);
#endif

  return enque(&queue1,(MESSAGE*)&data); 
}

bool dequeUart(void)
{
  if(deque(&queue1, &outBufer))
  {
#if DEBUG_QUEUE
    sendUART("d_ l:%d e:%d b:%d\r\n", queue1.current_load, queue1.begin, queue1.end);
#endif
    return true;
  }
  return false;
}
