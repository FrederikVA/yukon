// move_logic.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "variables.h"
#include "move.h"

MoveState currentMove;  // actual global instance

void resetMoveState() {
    strcpy(currentMove.cardToMove, "");
    strcpy(currentMove.fromColumnOrField, "");
    strcpy(currentMove.toColumnOrField, "");
}

void inferCardToMoveFromColumn() {
    if (currentMove.cardToMove[0] != '\0') return;

    int fromCol = currentMove.fromColumnOrField[1] - '1';
    Pile *source = (currentMove.fromColumnOrField[0] == 'C') ? &columns[fromCol] : &foundations[fromCol];

    Card *lastFaceUp = NULL;
    Card *current = source->top;
    while (current) {
        if (current->face_up) {
            lastFaceUp = current;
        }
        current = current->next;
    }

    if (lastFaceUp) {
        currentMove.cardToMove[0] = lastFaceUp->rank;
        currentMove.cardToMove[1] = lastFaceUp->suit;
        currentMove.cardToMove[2] = '\0';
    }
}

int validateMoveInput(const char *input) {
    resetMoveState();

    if (strlen(input) < 5 || strstr(input, "->") == NULL) {
        printf("Invalid input format. Expected <from>-><to>\n");
        return 0;
    }

    // Split into from and to parts
    char from[10], to[10];
    if (sscanf(input, "%9[^-]->%9s", from, to) != 2) {
        printf("Failed to parse move input.\n");
        return 0;
    }

    // Set toColumnOrField
    strncpy(currentMove.toColumnOrField, to, 2);
    currentMove.toColumnOrField[2] = '\0';

    // Parse fromPart
    if (from[0] != 'C' && from[0] != 'F') {
        printf("Invalid source. Must start with C or F.\n");
        return 0;
    }

    if (from[1] < '1' || from[1] > (from[0] == 'C' ? '7' : '4')) {
        printf("Invalid %s number. Must be in valid range.\n", from[0] == 'C' ? "column" : "foundation");
        return 0;
    }

    // Set fromColumnOrField
    strncpy(currentMove.fromColumnOrField, from, 2);
    currentMove.fromColumnOrField[2] = '\0';

    // Check for optional card specifier (e.g., "C6:4H")
    if (from[2] == ':') {
        char rank = from[3];
        char suit = from[4];
        if (!((rank >= '2' && rank <= '9') || rank == 'T' || rank == 'J' || rank == 'Q' || rank == 'K' || rank == 'A')) {
            printf("Invalid card rank.\n");
            return 0;
        }
        if (!(suit == 'C' || suit == 'D' || suit == 'H' || suit == 'S')) {
            printf("Invalid card suit.\n");
            return 0;
        }

        currentMove.cardToMove[0] = rank;
        currentMove.cardToMove[1] = suit;
        currentMove.cardToMove[2] = '\0';
    } else {
        // No specific card given
        strcpy(currentMove.cardToMove, "");
    }

    return 1;
}

// Helper to get rank index (A=0, 2=1, ..., K=12)
int getRankIndex(char rank) {
    const char *ranks = "A23456789TJQK";
    const char *ptr = strchr(ranks, rank);
    return ptr ? (int)(ptr - ranks) : -1;
}

// Helper: This version allows for same colour, just have to be a different suit!
int isDifferentSuit(char suitA, char suitB) {
    return suitA != suitB;
}

// Find a card in a column. Returns pointer to the card, or NULL if not found
Card* findCardInColumn(Pile *pile, const char *cardCode) {
    Card *current = pile->top;
    while (current != NULL) {
        if (current->rank == cardCode[0] && current->suit == cardCode[1]) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int validateFromMove() {
    inferCardToMoveFromColumn();
    int fromCol = currentMove.fromColumnOrField[1] - '1';
    Pile *source = (currentMove.fromColumnOrField[0] == 'C') ? &columns[fromCol] : &foundations[fromCol];

    if (currentMove.cardToMove[0] != '\0') {
        Card *found = findCardInColumn(source, currentMove.cardToMove);
        if (!found) {
            printf("Card %s not found in source column.\n", currentMove.cardToMove);
            return 0;
        }
        if (!found->face_up) {
            printf("Card %s is not face up.\n", currentMove.cardToMove);
            return 0;
        }
    } else {
        // Check if column has at least one face-up card
        Card *current = source->top;
        while (current && !current->face_up) {
            current = current->next;
        }
        if (!current) {
            printf("No face-up cards to move from source.\n");
            return 0;
        }
    }
    return 1;
}

int validateToMove() {
    inferCardToMoveFromColumn();
    printf("🛠 validateToMove(): move = %s from %s to %s\n",
    currentMove.cardToMove,
    currentMove.fromColumnOrField,
    currentMove.toColumnOrField);

    int toCol = currentMove.toColumnOrField[1] - '1';
    Pile *target = (currentMove.toColumnOrField[0] == 'C') ? &columns[toCol] : &foundations[toCol];

    char moveRank = currentMove.cardToMove[0];
    char moveSuit = currentMove.cardToMove[1];
    int moveRankIndex = getRankIndex(moveRank);

    if (currentMove.toColumnOrField[0] == 'C') {
        // Moving to a column
        if (!target->top) {
            if (moveRank == 'K') return 1;
            printf("Only Kings can be placed in empty columns.\n");
            return 0;
        }
        Card *bottom = target->top;
        while (bottom->next != NULL) bottom = bottom->next;

        if (!isDifferentSuit(bottom->suit, moveSuit)) {
            printf("Target card must have a different suit.\n");
            return 0;
        }
        if (getRankIndex(bottom->rank) != moveRankIndex + 1) {
            printf("Target card must be one rank higher.\n");
            return 0;
        }
        return 1;
    } else {
        Card *last = target->top;
        if (!last) {
            if (moveRank == 'A') return 1;
            printf("Only Aces can be placed in empty foundations.\n");
            return 0;
        }
        while (last->next) last = last->next; // go to last card in foundation

        if (last->suit != moveSuit) {
            printf("Foundation must be same suit.\n");
            return 0;
        }
        if (getRankIndex(moveRank) != getRankIndex(last->rank) + 1) {
            printf("Card must be one rank higher than foundation top.\n");
            return 0;
        }
        return 1;
    }
}

int validateMove() {
    if (!validateFromMove()) return 0;
    if (!validateToMove()) return 0;
    return 1;
}

// _____ ACTUAL MOVE LOGIC _____ //


void executeMove() {
    inferCardToMoveFromColumn();
    int fromCol = currentMove.fromColumnOrField[1] - '1';
    int toCol = currentMove.toColumnOrField[1] - '1';

    Pile *source = (currentMove.fromColumnOrField[0] == 'C') ? &columns[fromCol] : &foundations[fromCol];
    Pile *target = (currentMove.toColumnOrField[0] == 'C') ? &columns[toCol] : &foundations[toCol];

    Card *movingStart = NULL;
    Card *prev = NULL;
    Card *current = source->top;

    if (currentMove.cardToMove[0] != '\0') {
        // Find the card specified
        while (current) {
            if (current->rank == currentMove.cardToMove[0] && current->suit == currentMove.cardToMove[1]) {
                movingStart = current;
                if (prev) {
                    prev->next = NULL;
                } else {
                    source->top = NULL;
                }
                break;
            }
            prev = current;
            current = current->next;
        }
    } else {
        // Move bottom-most face-up card
        Card *lastFaceUp = NULL;
        Card *beforeLastFaceUp = NULL;
        current = source->top;
        while (current) {
            if (current->face_up) {
                lastFaceUp = current;
                break;
            }
            beforeLastFaceUp = current;
            current = current->next;
        }
        movingStart = lastFaceUp;
        if (beforeLastFaceUp) {
            beforeLastFaceUp->next = NULL;
        } else {
            source->top = NULL;
        }
    }

    if (!movingStart) {
        printf("Error: No cards found to move!\n");
        return;
    }

    // Find the tail of the moving cards
    Card *tail = movingStart;
    while (tail->next) {
        tail = tail->next;
    }

    // Attach to the target pile
    if (!target->top) {
        target->top = movingStart;
    } else {
        Card *current = target->top;
        while (current->next) {
            current = current->next;
        }
        current->next = movingStart;
    }

    postMoveUpdate(source);
    winCondition();
}

void postMoveUpdate(Pile *source) {
    // After moving, flip the last card face up if needed
    if (!source->top) return; // Nothing left

    Card *current = source->top;
    while (current->next) {
        current = current->next;
    }

    if (!current->face_up) {
        current->face_up = 1;
        printf("Flipped card %c%c face up.\n", current->rank, current->suit);
    }
}

void winCondition() {
    if (currentMove.toColumnOrField[0] == 'F' && currentMove.cardToMove[0] == 'K') {
        int allFoundationsFull = 1;
        for (int i = 0; i < 4; i++) {
            Card *current = foundations[i].top;
            while (current && current->next) {
                current = current->next;
            }
            if (!current || current->rank != 'K') {
                allFoundationsFull = 0;
                break;
            }
        }
        if (allFoundationsFull) {
            printf(" Congratulations! You've won the game! ");
        }
    }
}
