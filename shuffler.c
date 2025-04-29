#include <stdio.h>
#include <stdlib.h>
#include "fileHandler.h"
#include "deck.h"
#include "variables.h"


int countDeck() {
    int count = 0;
    Card *current = deck;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void randomShuffle() {
    printf("\n--- Starting Random Shuffle ---\n");
    printf("Deck size before shuffle: %d\n", countDeck());

    if (countDeck() != 52) {
        printf("Error: Deck must contain exactly 52 cards to shuffle.\n");
        return;
    }

    Card* cards[52] = {NULL};
    int count = 0;
    Card* current = deck;

    printf("Copying deck to array...\n");
    while (current != NULL) {
        cards[count++] = current;
        current = current->next;
    }
    
    printf("Deck copied to array. Card count = %d\n", count);

    printf("Shuffling deck (Fisher-Yates)...\n");
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card* temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }

    // Safety clear pointers
    printf("Clearing .next pointers...\n");
    for (int i = 0; i < count; i++) {
        cards[i]->next = NULL;
    }

    printf("Rebuilding linked list...\n");
    for (int i = 0; i < count - 1; i++) {
        cards[i]->next = cards[i + 1];
    }
    cards[count - 1]->next = NULL;
    deck = cards[0];

    printf("Deck size after shuffle: %d\n", countDeck());
    if (countDeck() != 52) {
        printf("Error: Deck corrupted after random shuffle.\n");
        return;
    }
    printf("--- Random Shuffle Done ---\n\n");
}

void riffleShuffle(int split) {
    printf("\n--- Starting Riffle Shuffle ---\n");
    printf("Deck size before shuffle: %d\n", countDeck());

    if (countDeck() != 52) {
        printf("Error: Deck must contain exactly 52 cards to shuffle.\n");
        return;
    }

    if (split <= 0 || split >= 52) {
        printf("Invalid split position.\n");
        return;
    }

    Card* cards[52] = {NULL};
    int count = 0;
    Card* current = deck;
    
    printf("Copying deck to array...\n");
    while (current != NULL) {
        cards[count++] = current;
        current = current->next;
    }

    printf("Deck copied to array. Card count = %d\n", count);

    Card* first[52] = {NULL};
    Card* second[52] = {NULL};

    for (int i = 0; i < split; i++) {
        first[i] = cards[i];
    }
    for (int i = split; i < 52; i++) {
        second[i - split] = cards[i];
    }

    // Safety clear pointers
    printf("Clearing .next pointers...\n");
    for (int i = 0; i < count; i++) {
        cards[i]->next = NULL;
    }
    
    printf("Rebuilding linked list (interleaving)...\n");
    int first_index = 0, second_index = 0;
    Card* newDeck = NULL;
    Card** tail = &newDeck;

    while (first_index < split || second_index < (52 - split)) {
        if (first_index < split) {
            *tail = first[first_index++];
            tail = &((*tail)->next);
        }
        if (second_index < (52 - split)) {
            *tail = second[second_index++];
            tail = &((*tail)->next);
        }
    }

    *tail = NULL;
    deck = newDeck;

    printf("Deck size after riffle shuffle: %d\n", countDeck());
    if (countDeck() != 52) {
        printf("Error: Deck corrupted after riffle shuffle.\n");
        return;
    }
    printf("--- Riffle Shuffle Done ---\n\n");
}

void printDeckDebug() {
    printf("DEBUG: Printing deck after shuffle:\n");
    Card *current = deck;
    int count = 0;
    while (current != NULL) {
        printf("%c%c ", current->rank, current->suit);
        current = current->next;
        count++;
    }
    printf("\nTotal cards in deck: %d\n", count);
}


