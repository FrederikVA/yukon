#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "structures.h"
#include "variables.h"
#include "deck.h"
#include "fileHandler.h"

// Appends dealt cards to a tableau column while keeping that column as a linked list.
static void appendCardToColumn(int col, Card *card) {
    if (columns[col].top == NULL) {
        columns[col].top = card;
        return;
    }

    Card *tail = columns[col].top;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = card;
}

// Implements P: deals the whole deck row-wise into Yukon column sizes 1, 6, 7, 8, 9, 10, 11.
void initGame() {
    clearColumns();
    clearFoundations();

    Card *current = deck;
    int col_sizes[7] = {1, 6, 7, 8, 9, 10, 11}; // total cards per column

    for (int row = 0; row < 11; row++) {
        for (int col = 0; col < 7; col++) {
            if (row >= col_sizes[col]) {
                continue;
            }
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
            copy->face_up = (row < col) ? 0 : 1;
            copy->next = NULL;

            appendCardToColumn(col, copy);
            current = current->next;
        }
    }
}
