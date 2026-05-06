#include <stdio.h>
#include <string.h>
#include "variables.h"
#include "printer.h"
#include "stateHandler.h"
#include "timer.h"
#include "gui.h"  

int main(int argc, char *argv[]) {
    loadBestCompletionTime();

    // GUI mode if --gui is passed (./yukon --gui)
    if (argc > 1 && strcmp(argv[1], "--gui") == 0) {
        printf("Launching GUI mode...\n");
        runGUI();  // SDL2 entry
        return 0;
    }

    // Terminal mode (default)
    char input[100];
    strcpy(lastCommand, "");
    strcpy(message, "");

    while (running) {
        printToTerminal();
        printf("INPUT > ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            running = 0;
        } else {
            input[strcspn(input, "\n")] = 0;
            handleCommand(input);
        }
    }

    printf("Goodbye!\n");
    return 0;
}
