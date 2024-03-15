#ifndef __WORK_STATE_H
#define __WORK_STATE_H

typedef enum
{
  MENU = 0, 
  GAME1,
  GAME2,
  END_GAME
} STATE;

STATE getWorkState(void);
void setWorkState(const STATE state);

#endif /* __WORK_STATE_H */