#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "variables.h"
#include "fileHandler.h"
#include "printer.h"
#include "deck.h"
#include "shuffler.h"
#include "game.h"
#include "move.h"
#include "history.h"
#include "timer.h"

static int parsePositiveInt(const char *text, int *value) {
    int result = 0;

    if (text == NULL || text[0] == '\0') {
        return 0;
    }

    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] < '0' || text[i] > '9') {
            return 0;
        }
        result = result * 10 + (text[i] - '0');
    }

    *value = result;
    return 1;
}

void handleCommand(const char *input) {
    strcpy(lastCommand, input);

    if (currentPhase == STARTUP) {
        if (strncmp(input, "LD", 2) == 0) {
            if (input[2] == '\0') {
                // Just "LD" with no filename (exactly 2 letters)
                if (loadDeck(NULL)) {
                    strcpy(message, "OK");
                } else {
                    strcpy(message, "Error: Could not load default deck.");
                }
            } else if (input[2] == ' ') {
                // "LD something"
                const char *filename = input + 3; // skip "LD "
                
                // Check if filename is single word (no space inside)
                if (strchr(filename, ' ') == NULL) {
                    if (loadDeck(filename)) {
                        strcpy(message, "OK");
                    } else {
                        const char *fileError = getLastFileError();
                        if (fileError[0] != '\0') {
                            snprintf(message, sizeof(message), "Error: Could not load deck: %s - %s", filename, fileError);
                        } else {
                            snprintf(message, sizeof(message), "Error: Could not load deck: %s", filename);
                        }
                    }
                } else {
                    strcpy(message, "Error: Invalid filename (no spaces allowed).");
                }
            } else {
                strcpy(message, "Error: Invalid LD command format.");
            }
        } else if (strcmp(input, "SW") == 0) {
            if (deck == NULL) {
                strcpy(message, "Error: No deck loaded.");
            } else {
                showDeck();
                strcpy(message, "OK");
            }
        } else if (strcmp(input, "QQ") == 0) {
            running = 0;
            strcpy(message, "Quitting program...");
        } else if (strcmp(input, "SR") == 0) {
            if (deck == NULL) {
                strcpy(message, "Error: No deck loaded.");
            } else {
                randomShuffle();
                reloadColumnsFromDeck();
                showDeck();
                strcpy(message, "OK");
            }
        } else if (strncmp(input, "SI", 2) == 0) {
            if (deck == NULL) {
                strcpy(message, "Error: No deck loaded.");
            } else {
                int split = 0;
                int validSplit = 1;
                if (input[2] == '\0') {
                    split = 26;
                } else if (input[2] == ' ') {
                    validSplit = parsePositiveInt(input + 3, &split);
                } else {
                    validSplit = 0;
                }

                if (!validSplit || split <= 0 || split >= countDeck()) {
                    strcpy(message, "Error: Invalid split position.");
                } else {
                    riffleShuffle(split);
                    reloadColumnsFromDeck();
                    showDeck();
                    strcpy(message, "OK");
                }
            }
        } else if (strcmp(input, "help") == 0) {
            printHelp();
            strcpy(message, "Help displayed.");
        } else if (strncmp(input, "SD", 2) == 0) {
            if (input[2] == '\0') {
                if (saveDeckToFile(NULL)) {
                    strcpy(message, "OK");
                } else {
                    strcpy(message, "Error: Could not save deck.");
                }
            } else if (input[2] == ' ') {
                const char *filename = input + 3;
                if (strchr(filename, ' ') == NULL) {
                    if (saveDeckToFile(filename)) {
                        strcpy(message, "OK");
                    } else {
                        strcpy(message, "Error: Could not save deck.");
                    }
                } else {
                    strcpy(message, "Error: Invalid filename (no spaces allowed).");
                }
            } else {
                strcpy(message, "Error: Invalid SD command format.");
            }
        } else if (strncmp(input, "L", 1) == 0) {
            if (input[1] == ' ' && strchr(input + 2, ' ') == NULL) {
                if (loadGameState(input + 2)) {
                    clearHistory();
                    strcpy(message, "OK");
                } else {
                    strcpy(message, "Error: Could not load game state.");
                }
            } else {
                strcpy(message, "Error: Invalid L command format.");
            }
        } else if (strcmp(input, "show") == 0) {
            showDeck();
            strcpy(message, "Deck shown.");
        } else if (strcmp(input, "clear") == 0) {
            clearColumns();
            strcpy(message, "Columns cleared.");
        } else if (strcmp(input, "reload") == 0) {
            reloadColumnsFromDeck();
            strcpy(message, "Columns reloaded from deck.");
        } else if (strcmp(input, "exit") == 0) {
            running = 0;
            strcpy(message, "Exiting...");
        } else if (strcmp(input, "P") == 0) {
            if (deck == NULL) {
                strcpy(message, "Error: No deck loaded.");
            } else {
                currentPhase = PLAY;
                initGame();
                clearHistory();
                startGameTimer();
                strcpy(message, "OK");
            }
        } else {
            strcpy(message, "Unknown command in STARTUP phase.");
        }

    } else if (currentPhase == PLAY) {
        if (strcmp(input, "Q") == 0) {
            currentPhase = STARTUP;
            clearColumns();
            clearFoundations();
            clearHistory();
            stopGameTimer();
            strcpy(message, "Returned to startup.");
        } else if (strcmp(input, "LD") == 0 ||
                   strncmp(input, "LD ", 3) == 0 ||
                   strcmp(input, "SW") == 0 ||
                   strcmp(input, "SR") == 0 ||
                   strncmp(input, "SI", 2) == 0 ||
                   strncmp(input, "SD", 2) == 0 ||
                   strcmp(input, "P") == 0 ||
                   strcmp(input, "QQ") == 0) {
            strcpy(message, "Command not available in the PLAY phase.");
        } else if (strstr(input, "->")) {
            if (validateMoveInput(input) && validateMove()) {
                executeMove();
                if (isGameWon()) {
                    strcpy(message, "Congratulations! You've won the game!");
                } else {
                    strcpy(message, "Move executed successfully!");
                }
            } else {
                strcpy(message, "Move input was invalid or not allowed.");
            }
        } else {
            if (strcmp(input, "U") == 0) {
                if (undoMove()) {
                    strcpy(message, "OK");
                } else {
                    strcpy(message, "Error: No move to undo.");
                }
            } else if (strcmp(input, "R") == 0) {
                if (redoMove()) {
                    strcpy(message, "OK");
                } else {
                    strcpy(message, "Error: No move to redo.");
                }
            } else if (strncmp(input, "S", 1) == 0) {
                if (input[1] == ' ' && strchr(input + 2, ' ') == NULL) {
                    if (saveGameState(input + 2)) {
                        strcpy(message, "OK");
                    } else {
                        strcpy(message, "Error: Could not save game state.");
                    }
                } else {
                    strcpy(message, "Error: Invalid S command format.");
                }
            } else if (strncmp(input, "L", 1) == 0) {
                if (input[1] == ' ' && strchr(input + 2, ' ') == NULL) {
                    if (loadGameState(input + 2)) {
                        clearHistory();
                        strcpy(message, "OK");
                    } else {
                        strcpy(message, "Error: Could not load game state.");
                    }
                } else {
                    strcpy(message, "Error: Invalid L command format.");
                }
            } else {
                strcpy(message, "Unknown command in PLAY phase.");
            }
        }
    }
}    
