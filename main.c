#include <stdio.h>
#include <string.h>
#include "variables.h"
#include "printer.h"
#include "deck.h"
#include "stateHandler.h"
#include "gui.h"  

int main(int argc, char *argv[]) {
    createDefaultDeck();

    // GUI mode if --gui is passed (./yukon --gui)
    if (argc > 1 && strcmp(argv[1], "--gui") == 0) {
        printf("Launching GUI mode...\n");
        runGUI();  // SDL2 entry
        return 0;
    }

    // Terminal mode (default)
    char input[100];
    strcpy(lastCommand, "None");
    strcpy(message, "Welcome to Yukon! Type 'help' for commands.");

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
