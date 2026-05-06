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
    if (countDeck() != 52) {
        printf("Error: Deck must contain exactly 52 cards to shuffle.\n");
        return;
    }

    int count = 0;
    Card *shuffled = NULL;

    while (deck != NULL) {
        Card *card = deck;
        deck = deck->next;
        card->next = NULL;

        int position = rand() % (count + 1);
        if (position == 0) {
            card->next = shuffled;
            shuffled = card;
        } else {
            Card *current = shuffled;
            for (int i = 1; i < position; i++) {
                current = current->next;
            }
            card->next = current->next;
            current->next = card;
        }
        count++;
    }

    deck = shuffled;
}

void riffleShuffle(int split) {
    if (countDeck() != 52) {
        printf("Error: Deck must contain exactly 52 cards to shuffle.\n");
        return;
    }

    if (split <= 0 || split >= 52) {
        printf("Invalid split position.\n");
        return;
    }

    Card *first = deck;
    Card *firstTail = deck;
    for (int i = 1; i < split; i++) {
        firstTail = firstTail->next;
    }

    Card *second = firstTail->next;
    firstTail->next = NULL;

    Card *newDeck = NULL;
    Card **tail = &newDeck;

    while (first != NULL || second != NULL) {
        if (first != NULL) {
            *tail = first;
            first = first->next;
            (*tail)->next = NULL;
            tail = &((*tail)->next);
        }
        if (second != NULL) {
            *tail = second;
            second = second->next;
            (*tail)->next = NULL;
            tail = &((*tail)->next);
        }
    }

    deck = newDeck;
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

