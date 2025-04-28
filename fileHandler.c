#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "variables.h"

extern const char ranks[];
extern const char suits[];

void createDeckFile(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Error creating deck file.\n");
        exit(1);
    }

    for (int s = 0; s < 4; s++) {
        for (int r = 0; r < 13; r++) {
            fprintf(f, "%c%c\n", ranks[r], suits[s]);
        }
    }
    fclose(f);
}

int countLinesInFile(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;

    int count = 0;
    char line[10];
    while (fgets(line, sizeof(line), f)) {
        count++;
    }
    fclose(f);
    return count;
}

void loadBoardFromFile(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Failed to open deck file. Did you load in a deck?\n");
        return;
    }

    char buffer[10];
    int card_count = 0;

    // Clean columns first
    for (int i = 0; i < 7; i++) {
        columns[i].top = NULL;
    }

    while (fgets(buffer, sizeof(buffer), f)) {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) != 2) {
            printf("Invalid card format: %s\n", buffer);
            fclose(f);
            return;
        }

        Card *new_card = (Card *)malloc(sizeof(Card));
        if (!new_card) {
            printf("Memory allocation failed!\n");
            exit(1);
        }
        new_card->rank = buffer[0];
        new_card->suit = buffer[1];
        new_card->face_up = 0; // always face down at load
        new_card->next = NULL;

        // Distribute cards into columns (IN END OF COLUMN)
        int col = card_count % 7;
        if (columns[col].top == NULL) {
            columns[col].top = new_card;
        } else {
            Card *current = columns[col].top;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_card;
        }
        card_count++;
    }

    fclose(f);

    if (card_count != 52) {
        printf("Error: Deck does not contain 52 cards.\n");
    } else {
        printf("Deck loaded successfully!\n");
    }
}

void loadDeck(const char *filename) {
    if (filename == NULL || strcmp(filename, "") == 0) {
        filename = "cards.txt"; // default filename
    }

    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Deck file does not exist. Creating default deck file: %s\n", filename);
        createDeckFile(filename);
    } else {
        fclose(f); // We only opened to check existence
    }

    int lines = countLinesInFile(filename);
    if (lines != 52) {
        printf("Deck file is invalid (expected 52 lines, found %d)\n", lines);
        return;
    }

    loadBoardFromFile(filename);
}

void showDeck() {
    // Turn all cards face up
    for (int i = 0; i < 7; i++) {
        Card *current = columns[i].top;
        while (current) {
            current->face_up = 1;
            current = current->next;
        }
    }
}

void saveDeckToFile(const char *filename) {
    if (deck == NULL) {
        printf("No deck to save.\n");
        return;
    }

    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Failed to open file for writing: %s\n", filename);
        return;
    }

    Card *current = deck;
    while (current != NULL) {
        fprintf(f, "%c%c\n", current->rank, current->suit);
        current = current->next;
    }

    fclose(f);
    printf("Deck saved to file: %s\n", filename);
}

void reloadColumnsFromDeck() {
    // Clear existing columns
    for (int i = 0; i < 7; i++) {
        columns[i].top = NULL;
    }

    // Fill columns row-by-row
    Card *current = deck;
    int card_count = 0;
    while (current != NULL) {
        int col = card_count % 7;
        
        // Add at END of each column
        if (columns[col].top == NULL) {
            columns[col].top = current;
        } else {
            Card *tail = columns[col].top;
            while (tail->next != NULL) {
                tail = tail->next;
            }
            tail->next = current;
        }

        Card *next = current->next;
        current->next = NULL; // Very important: cut off next chain
        current = next;

        card_count++;
    }
}
