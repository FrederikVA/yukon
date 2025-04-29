#ifndef VARIABLES_H
#define VARIABLES_H

#include "structures.h"

extern Pile columns[7];
extern Pile foundations[4];
extern GamePhase currentPhase;
extern int running;
extern Card *deck;

extern char lastCommand[100];
extern char message[200];
extern char currentFile[100];

extern char cardToMove[3];
extern char fromColumnOrField[3];
extern char toColumnOrField[3];

#endif
