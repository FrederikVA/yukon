#include <stddef.h>
#include <string.h>
#include "variables.h"
#include "fileHandler.h"
#include "printer.h"
#include "deck.h"
#include "shuffler.h"

void handleCommand(const char *input) {
    strcpy(lastCommand, input);

    if (currentPhase == STARTUP) {
        if (strcmp(input, "LD") == 0) {
            loadDeck(NULL);
            strcpy(message, "Deck loaded.");
        } else if (strcmp(input, "SW") == 0) {
            showDeck();
            strcpy(message, "Deck shown.");
        } else if (strcmp(input, "P") == 0) {
            currentPhase = PLAY;
            strcpy(message, "Game started!");
        } else if (strcmp(input, "QQ") == 0) {
            running = 0;
            strcpy(message, "Quitting program...");
        } else if (strcmp(input, "SR") == 0) {
            randomShuffle();
            saveDeckToFile("cards.txt");
            reloadColumnsFromDeck();
            strcpy(message, "Deck shuffled randomly and saved.");
        }
        else if (strncmp(input, "SI", 2) == 0) {
            riffleShuffle(26);
            saveDeckToFile("cards.txt");
            reloadColumnsFromDeck(); 
            strcpy(message, "Deck riffle shuffled and saved.");
        }
         else if (strcmp(input, "help") == 0) {
            printHelp();
            strcpy(message, "Help displayed.");
        } else {
            strcpy(message, "Unknown command in STARTUP phase.");
        }
    }
    else if (currentPhase == PLAY) {
        if (strcmp(input, "Q") == 0) {
            currentPhase = STARTUP;
            strcpy(message, "Returned to startup.");
        } else {
            strcpy(message, "Unknown command in PLAY phase.");
        }
    }
}
