#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "variables.h"
#include "fileHandler.h"
#include "printer.h"
#include "deck.h"
#include "shuffler.h"
#include "game.h"

void handleCommand(const char *input) {
    strcpy(lastCommand, input);

    if (currentPhase == STARTUP) {
        if (strncmp(input, "LD", 2) == 0) {
            if (input[2] == '\0') {
                // Just "LD" with no filename (exactly 2 letters)
                loadDeck(NULL);
                strcpy(message, "Deck loaded (default).");
            } else if (input[2] == ' ') {
                // "LD something"
                const char *filename = input + 3; // skip "LD "
                
                // Check if filename is single word (no space inside)
                if (strchr(filename, ' ') == NULL) {
                    loadDeck(filename); // Load with provided filename
                    snprintf(message, sizeof(message), "Deck loaded (custom file): %s", filename);
                } else {
                    strcpy(message, "Error: Invalid filename (no spaces allowed).");
                }
            } else {
                strcpy(message, "Error: Invalid LD command format.");
            }
        } else if (strcmp(input, "SW") == 0) {
            showDeck();
            strcpy(message, "Deck shown.");
        } else if (strcmp(input, "QQ") == 0) {
            running = 0;
            strcpy(message, "Quitting program...");
        } else if (strcmp(input, "SR") == 0) {
            randomShuffle();
            printDeckDebug();
            saveDeckToFile();
            loadDeck(NULL); 
            reloadColumnsFromDeck();
            showDeck(); 
            strcpy(message, "Deck shuffled randomly and saved.");
        } else if (strncmp(input, "SI", 2) == 0) {
            riffleShuffle(26);
            saveDeckToFile();
            loadDeck(NULL); 
            reloadColumnsFromDeck();
            showDeck();
            strcpy(message, "Deck riffle shuffled and saved.");
        } else if (strcmp(input, "help") == 0) {
            printHelp();
            strcpy(message, "Help displayed.");
        } else if (strcmp(input, "SD") == 0) {
            showDeckFiles();
            strcpy(message, "printed deck names");
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
            printf("Enters initgame!\n");
            currentPhase = PLAY;
            initGame();
            printf("Exits initgame!\n");
            strcpy(message, "Game started!");
        } else {
            strcpy(message, "Unknown command in STARTUP phase.");
        }

    } else if (currentPhase == PLAY) {
        if (strcmp(input, "Q") == 0) {
            currentPhase = STARTUP;
            strcpy(message, "Returned to startup.");
        } else if (strstr(input, "->") != NULL) {
            // It's a move command
            char fromPart[20];
            char toPart[20];
        
            if (sscanf(input, "%19[^-]->%19s", fromPart, toPart) == 2) {
        
                // TODO: Call moveCard(fromPart, toPart) or similar
            } else {
                strcpy(message, "Invalid move command format.");
            }
        } else {
            strcpy(message, "Unknown command in PLAY phase.");
        }
    }
}
