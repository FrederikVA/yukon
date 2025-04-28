#include <stdio.h>
#include <stdlib.h>
#include "fileHandler.h"
#include "deck.h"
#include "variables.h"

// Function to shuffle the deck using the riffle shuffle method
void riffleShuffle(int split) {
    if (deck == NULL) {
        printf("Deck is empty. Cannot shuffle.\n");
        return;
    }

    if (split <= 0 || split >= 52) {
        printf("Invalid split position.\n");
        return;
    }

    // Step 1: Split deck
    Card* first = deck;
    Card* second = NULL;
    Card* current = deck;
    int count = 0;

    while (current != NULL && count < split - 1) {
        current = current->next;
        count++;
    }

    if (current != NULL) {
        second = current->next;
        current->next = NULL; // Break into two lists
    }

    // Step 2: Interleave cards
    Card* newDeck = NULL;
    Card** tail = &newDeck; // Tail pointer to last card in new deck

    while (first != NULL || second != NULL) {
        if (first != NULL) {
            *tail = first;
            tail = &((*tail)->next);
            first = first->next;
        }
        if (second != NULL) {
            *tail = second;
            tail = &((*tail)->next);
            second = second->next;
        }
    }

    *tail = NULL;
    deck = newDeck;
}


void randomShuffle() {
    if (deck == NULL) {
        printf("Deck is empty. Cannot shuffle.\n");
        return;
    }

    // Step 1: Copy deck to array
    Card* cards[52];
    int count = 0;
    Card* current = deck;
    while (current != NULL && count < 52) {
        cards[count++] = current;
        current = current->next;
    }

    // Step 2: Fisher-Yates shuffle
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card* temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }

    // Step 3: Rebuild deck
    for (int i = 0; i < count - 1; i++) {
        cards[i]->next = cards[i + 1];
    }
    cards[count - 1]->next = NULL;
    deck = cards[0];
}

