#ifndef __ARKANOID_GAME_ENGINE_H
#define __ARKANOID_GAME_ENGINE_H

#define LC_INCLUDE "lc-addrlabels.h"
#include "pt.h"

PT_THREAD(ArcanoidGameEngineThread(struct pt *pt));

void arkanoidInitGame(void);

#endif /* __ARKANOID_GAME_ENGINE_H */