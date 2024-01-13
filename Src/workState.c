#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "workState.h"

static STATE workState = MENU;

STATE getWorkState(void)
{
  return workState;
}

void setWorkState(const STATE state)
{
  workState = state;
}