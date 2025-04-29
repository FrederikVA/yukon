#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "variables.h"
#include "deck.h"
#include "fileHandler.h"

void debugPrintColumnSizes() {
    printf("=== Column Sizes ===\n");
    for (int i = 0; i < 7; i++) {
        int count = 0;
        Card *current = columns[i].top;
        while (current != NULL) {
            count++;
            current = current->next;
        }
        printf("C%d: %d cards\n", i + 1, count);
    }
    printf("====================\n");
}

void initGame() {
    clearColumns();

    Card *current = deck;
    int col_sizes[7] = {1, 6, 7, 8, 9, 10, 11}; // total cards per column

    for (int col = 0; col < 7; col++) {
        int totalCards = col_sizes[col];
        int hiddenCards = col; // Column 0 has 0 hidden, column 1 has 1 hidden, etc.

        for (int i = 0; i < totalCards; i++) {
            if (current == NULL) {
                printf("Error: Not enough cards in deck to initialize board.\n");
                exit(1);
            }

            // Make a copy of the card
            Card *copy = (Card *)malloc(sizeof(Card));
            if (!copy) {
                printf("Memory allocation failed!\n");
                exit(1);
            }

            copy->rank = current->rank;
            copy->suit = current->suit;
            copy->face_up = (i < hiddenCards) ? 0 : 1;
            copy->next = NULL;

            // Add to column
            if (columns[col].top == NULL) {
                columns[col].top = copy;
            } else {
                Card *tail = columns[col].top;
                while (tail->next != NULL) {
                    tail = tail->next;
                }
                tail->next = copy;
            }

            current = current->next;
        }
    }
    debugPrintColumnSizes();
}

