#ifndef MOVE_H
#define MOVE_H

#include "structures.h" 

typedef struct {
    char cardToMove[3];         // e.g. "4H"
    char fromColumnOrField[3];  // e.g. "C6"
    char toColumnOrField[3];    // e.g. "C4"
} MoveState;

extern MoveState currentMove;

// Reset and validate input
void resetMoveState();
int validateMoveInput(const char *input);

// Game logic
int validateMove();
int validateFromMove();
int validateToMove();
void inferCardToMoveFromColumn();

// Helpers
int getRankIndex(char rank);
int isDifferentSuit(char suitA, char suitB);
Card* findCardInColumn(Pile *pile, const char *cardCode);

// Execute move
void executeMove();
void postMoveUpdate(Pile *source);
void winCondition();

#endif
