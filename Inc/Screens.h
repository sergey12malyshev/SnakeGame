#ifndef __Screens_H
#define __Screens_H

void createFood(uint16_t x0, uint16_t y0, const uint16_t sizeFood);
void deleteFood(uint16_t x0, uint16_t y0, const uint16_t sizeFood);
void createWalls(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void createWorkRegion(void);
void screenSaver(void);
void screenEndGame(void);
void screenGameCompleted(void);
void screenOverVoltageError(void);
void screenUnderVoltageError(void);
void batterySumbolShow(void);
void createMonster(uint8_t object_number, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor, uint16_t bgcolor);

#endif /* __Screens_H */
