#include <stdio.h>
#include "constants.h"
#include "variables.h"
#include "printer.h"

void printToTerminal() {
    //printf("\033[H\033[J");
    printBoard();
    printLastCommand();
    printMessage();
}

void printBoard() {
    // 1. Print header
    printf("%s", WALL);
    for (int i = 0; i < 7; i++) {
        printf(" C%d %s", i + 1, WALL);
    }
    printf("\n");
    printf("------------------------------------\n");

    // 2. Find maximum column height
    int max_height = 0;
    for (int i = 0; i < 7; i++) {
        int count = 0;
        Card *current = columns[i].top;
        while (current) {
            count++;
            current = current->next;
        }
        if (count > max_height) {
            max_height = count;
        }
    }

    if (max_height < 4) {
        max_height = 4;
    }

    // 3. Print rows
    for (int row = 0; row < max_height; row++) {
        printf("%s", WALL);
        for (int col = 0; col < 7; col++) {
            // Find the card at this row
            Card *current = columns[col].top;
            int current_row = 0;
            while (current && current_row < row) {
                current = current->next;
                current_row++;
            }
            if (current) {
                if (current->face_up) {
                    printf(" %c%c %s", current->rank, current->suit, WALL);
                } else {
                    printf(" [] %s", WALL);
                }
            } else {
                printf(" %s%s", EMPTY_SPOT, WALL);
            }
        }

        if (row < 4) {
            Card *current = foundations[row].top;
            while (current && current->next) {
                current = current->next;
            }
            if (current) {
                printf("  F%d [ %c%c ]", row + 1, current->rank, current->suit);
            } else {
                printf("  F%d [    ]", row + 1);
            }
        }        
        printf("\n");
    }
}

void printLastCommand() {
    printf("LAST Command: %s\n", lastCommand);
}

void printMessage() {
    printf("Message: %s\n", message);
}

void printInputPrompt() {
    printf("INPUT > ");
}

void refreshScreen() {
    printBoard();
    printLastCommand();
    printMessage();
    printInputPrompt();
}

void printHelp() {
    printf("\nAvailable commands:\n");
    printf("LD — Load a deck\n");
    printf("SW — Show deck\n");
    printf("SI — Shuffle deck (split)\n");
    printf("SR — Shuffle deck (random)\n");
    printf("SD — Save deck\n");
    printf("P — Play (start game)\n");
    printf("Q — Quit playing (return to startup)\n");
    printf("QQ — Quit program\n\n");
    printf("SD - Show deck files\n");
}

