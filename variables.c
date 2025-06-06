#include <stddef.h>
#include "structures.h"
#include "variables.h"

// Global piles
Pile columns[7];
Pile foundations[4];

// Game phase
GamePhase currentPhase = STARTUP;

// Run control
int running = 1;

// Deck of cards
Card *deck = NULL;

// For printing
char lastCommand[100];
char message[200];

// Working file / deck
char currentFile[100] = "cards.txt";  // Default deck name

char cardToMove[3] = ""; // Card to move
char fromColumnOrField[3] = ""; // From column or field
char toColumnOrField[3] = ""; // To column or field