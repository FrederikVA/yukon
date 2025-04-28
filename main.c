#include <stdio.h>
#include <string.h>
#include "variables.h"
#include "printer.h"
#include "deck.h"
#include "stateHandler.h"


int main() {
    char input[100];

    strcpy(lastCommand, "None");
    strcpy(message, "Welcome to Yukon! Type 'help' for commands.");
    createDefaultDeck();

    while (running) {
        printToTerminal();
        if (currentPhase == STARTUP) {
            printf("[STARTUP] > ");
        } else {
            printf("INPUT > ");
        }

        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = 0;
            handleCommand(input); 
        }
    }

    printf("Goodbye!\n");
    return 0;
}
