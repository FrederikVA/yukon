#ifndef TIMER_H
#define TIMER_H

void startGameTimer(void);
void stopGameTimer(void);
void setGameElapsed(long seconds, int active);
long getGameElapsed(void);
void loadBestCompletionTime(void);
void updateBestCompletionTime(long seconds);

#endif
