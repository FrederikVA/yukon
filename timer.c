#include <stdio.h>
#include <time.h>
#include "variables.h"
#include "timer.h"

extern time_t gameStartTime;

void startGameTimer(void) {
    gameElapsedBeforePause = 0;
    gameStartTime = time(NULL);
    gameTimerActive = 1;
}

void stopGameTimer(void) {
    if (gameTimerActive) {
        gameElapsedBeforePause = getGameElapsed();
        gameTimerActive = 0;
    }
}

void setGameElapsed(long seconds, int active) {
    gameElapsedBeforePause = seconds;
    gameTimerActive = active;
    if (active) {
        gameStartTime = time(NULL) - seconds;
    }
}

long getGameElapsed(void) {
    if (!gameTimerActive) {
        return gameElapsedBeforePause;
    }
    return gameElapsedBeforePause + (long)(time(NULL) - gameStartTime);
}

void loadBestCompletionTime(void) {
    FILE *f = fopen("best_time.txt", "r");
    if (!f) {
        bestCompletionTime = 0;
        return;
    }

    if (fscanf(f, "%ld", &bestCompletionTime) != 1) {
        bestCompletionTime = 0;
    }
    fclose(f);
}

void updateBestCompletionTime(long seconds) {
    if (seconds <= 0) {
        return;
    }
    if (bestCompletionTime != 0 && seconds >= bestCompletionTime) {
        return;
    }

    bestCompletionTime = seconds;
    FILE *f = fopen("best_time.txt", "w");
    if (!f) {
        return;
    }
    fprintf(f, "%ld\n", bestCompletionTime);
    fclose(f);
}
