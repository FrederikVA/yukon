#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "structures.h"
#include "variables.h"
#include "shuffler.h"

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
    if (filename != NULL && strcmp(filename, "") != 0) {
        snprintf(currentFile, sizeof(currentFile), "%s.txt", filename); // Save new filename
    }
    char fullPath[200];
    snprintf(fullPath, sizeof(fullPath), "decks/%s", currentFile);

    FILE *f = fopen(fullPath, "r");
    if (!f) {
        printf("Deck file does not exist. Creating default deck file: %s\n", fullPath);
        createDeckFile(fullPath);
    } else {
        fclose(f);
    }

    int lines = countLinesInFile(fullPath);
    if (lines != 52) {
        printf("Deck file is invalid (expected 52 lines, found %d)\n", lines);
        return;
    }

    loadBoardFromFile(fullPath);
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

void showDeckFiles() {
    struct dirent *entry;
    DIR *dp = opendir("decks");

    if (dp == NULL) {
        printf("Error: Could not open decks folder.\n");
        return;
    }

    printf("Available decks:\n");
    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) { // Regular file
            printf(" - %s\n", entry->d_name);
        }
    }

    closedir(dp);
}

void saveDeckToFile() {
    if (deck == NULL) {
        printf("No deck to save.\n");
        return;
    }

    if (countDeck() != 52) {
        printf("Deck is not valid (not 52 cards). Save cancelled.\n");
        return;
    }

    char fullPath[200];
    snprintf(fullPath, sizeof(fullPath), "decks/%s", currentFile);

    FILE *f = fopen(fullPath, "w");
    if (!f) {
        printf("Failed to open file for writing: %s\n", fullPath);
        return;
    }

    Card *current = deck;
    while (current != NULL) {
        fprintf(f, "%c%c\n", current->rank, current->suit);
        current = current->next;
    }

    fclose(f);
    printf("Deck saved to file: %s\n", fullPath);
}

int validateDeckFile(const char *filename) {
    const char *validRanks = "A23456789TJQK";
    const char *validSuits = "CDHS"; // Clubs, Diamonds, Hearts, Spades

    int cardSeen[13][4] = {0}; // [rank][suit] table

    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error opening file for validation: %s\n", filename);
        return 0;
    }

    char line[10];
    int lineCount = 0;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0; // Remove newline
        if (strlen(line) != 2) {
            printf("Invalid card format: '%s'\n", line);
            fclose(f);
            return 0;
        }

        char rank = line[0];
        char suit = line[1];

        const char *rankPtr = strchr(validRanks, rank);
        const char *suitPtr = strchr(validSuits, suit);

        if (!rankPtr || !suitPtr) {
            printf("Invalid card detected: '%s'\n", line);
            fclose(f);
            return 0;
        }

        int rankIndex = rankPtr - validRanks;
        int suitIndex = suitPtr - validSuits;

        if (cardSeen[rankIndex][suitIndex]) {
            printf("Duplicate card detected: '%s'\n", line);
            fclose(f);
            return 0;
        }

        cardSeen[rankIndex][suitIndex] = 1;
        lineCount++;
    }

    fclose(f);

    if (lineCount != 52) {
        printf("Deck does not contain exactly 52 cards (found %d).\n", lineCount);
        return 0;
    }

    return 1; // OK
}

void clearColumns() {
    for (int i = 0; i < 7; i++) {
        Card *current = columns[i].top;
        while (current != NULL) {
            Card *next = current->next;
            free(current);
            current = next;
        }
        columns[i].top = NULL;
    }
}

void reloadColumnsFromDeck() {
    clearColumns();

    Card *current = deck;
    int card_count = 0;

    while (current != NULL) {
        int col = card_count % 7;

        // Copy the card (deep copy)
        Card *copy = (Card *)malloc(sizeof(Card));
        if (!copy) {
            printf("Memory allocation failed while copying card!\n");
            exit(1);
        }

        copy->rank = current->rank;
        copy->suit = current->suit;
        copy->face_up = current->face_up;
        copy->next = NULL;

        // Append to the end of the column
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
        card_count++;
    }
}
