#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "variables.h"

const char ranks[] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
const char suits[] = {'C', 'D', 'H', 'S'};

void createDefaultDeck() {
    Card *last = NULL;

    for (int s = 0; s < 4; s++) { // For each suit
        for (int r = 0; r < 13; r++) { // For each rank
            Card *new_card = (Card *)malloc(sizeof(Card));
            if (!new_card) {
                printf("Memory allocation failed!\n");
                exit(1);
            }
            new_card->rank = ranks[r];
            new_card->suit = suits[s];
            new_card->face_up = 0; // Default face down
            new_card->next = NULL;

            if (deck == NULL) {
                deck = new_card;
            } else {
                last->next = new_card;
            }
            last = new_card;
        }
    }
}
