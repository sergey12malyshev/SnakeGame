#ifndef __MENU_H
#define __MENU_H

bool getMenuState(void);
void setMenuState(const bool state);

bool mainMenu(void);
void screenMainMenu(void);
uint8_t getSpeedGame(void);

#endif /* __MENU_H */