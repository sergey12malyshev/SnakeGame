#include "main.h"
#include "filter.h"


/* Экспоненциальное бегущее среднее  filt = (A * filt + signal) >> k, https://alexgyver.ru/lessons/filters/ */
uint16_t expRunningAverageFilter(uint16_t input)
{
  static uint32_t filt = 0;
  const uint8_t k = 2;
  const uint8_t a = 3; // a =  (2^k) – 1

  filt = (a * filt + input) >> k;

  return filt;
}