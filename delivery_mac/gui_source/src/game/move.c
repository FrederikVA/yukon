// move_logic.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "variables.h"
#include "move.h"
#include "history.h"
#include "timer.h"

MoveState currentMove;  // actual global instance

// Clears the parsed move before reading another <from>-><to> command.
void resetMoveState() {
    strcpy(currentMove.cardToMove, "");
    strcpy(currentMove.fromColumnOrField, "");
    strcpy(currentMove.toColumnOrField, "");
}

// Supports moves like C6->C4 by resolving the implicit card to the bottom-most face-up card.
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

// Parses the required move grammar: C6:4H->C4, C6->F1, or F3->C6.
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
    if (to[0] != 'C' && to[0] != 'F') {
        printf("Invalid destination. Must start with C or F.\n");
        return 0;
    }
    if (to[1] < '1' || to[1] > (to[0] == 'C' ? '7' : '4') || to[2] != '\0') {
        printf("Invalid destination number. Must be in valid range.\n");
        return 0;
    }
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
        if (from[0] == 'F') {
            printf("Foundation moves must not include a card specifier.\n");
            return 0;
        }
        char rank = from[3];
        char suit = from[4];
        if (from[5] != '\0') {
            printf("Invalid source card format.\n");
            return 0;
        }
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
        if (from[2] != '\0') {
            printf("Invalid source format.\n");
            return 0;
        }
        // No specific card given
        strcpy(currentMove.cardToMove, "");
    }

    return 1;
}

// Helper to get rank index (A=0, 2=1, ..., K=12)
// Converts rank characters to ordered values so move rules can compare rank differences.
int getRankIndex(char rank) {
    const char *ranks = "A23456789TJQK";
    const char *ptr = strchr(ranks, rank);
    return ptr ? (int)(ptr - ranks) : -1;
}

// Helper: This version allows for same colour, just have to be a different suit!
// Yukon column rule from the PDF: moved card must be a different suit, not necessarily alternating color.
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

// Foundation rule: only the bottom card of a tableau column can be moved to a foundation.
static int isBottomCard(Pile *pile, Card *card) {
    Card *current = pile->top;
    if (!current || !card) return 0;

    while (current->next != NULL) {
        current = current->next;
    }
    return current == card;
}

// Validates that the source card exists and is face up before any board mutation happens.
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

// Validates destination rules for both tableau columns and foundations exactly before executing a move.
int validateToMove() {
    inferCardToMoveFromColumn();

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
        if (currentMove.fromColumnOrField[0] != 'C') {
            printf("Foundations can only receive cards from columns.\n");
            return 0;
        }

        int fromCol = currentMove.fromColumnOrField[1] - '1';
        Card *movingCard = findCardInColumn(&columns[fromCol], currentMove.cardToMove);
        if (!isBottomCard(&columns[fromCol], movingCard)) {
            printf("Only the bottom card in a column can move to a foundation.\n");
            return 0;
        }

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

// Combines source and destination validation so invalid moves never alter the linked-list board state.
int validateMove() {
    if (!validateFromMove()) return 0;
    if (!validateToMove()) return 0;
    return 1;
}

// Win condition: all four foundations must end at Kings, meaning every suit was built Ace through King.
int isGameWon(void) {
    for (int i = 0; i < 4; i++) {
        Card *current = foundations[i].top;
        while (current && current->next) {
            current = current->next;
        }
        if (!current || current->rank != 'K') {
            return 0;
        }
    }

    return 1;
}

// _____ ACTUAL MOVE LOGIC _____ //


// Executes a validated move by relinking nodes, preserving any card sequence below the moved card.
void executeMove() {
    inferCardToMoveFromColumn();
    recordUndoState();
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
        Card *previous = NULL;
        current = source->top;
        while (current) {
            if (current->face_up) {
                lastFaceUp = current;
                beforeLastFaceUp = previous;
            }
            previous = current;
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

// Yukon rule: when a move reveals a face-down card, flip the newly exposed bottom card face up.
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

// After foundation moves, update the timer/best-time state and show the winning message when complete.
void winCondition() {
    if (currentMove.toColumnOrField[0] == 'F' && currentMove.cardToMove[0] == 'K') {
        if (isGameWon()) {
            long elapsed = getGameElapsed();
            updateBestCompletionTime(elapsed);
            stopGameTimer();
            strcpy(message, "Congratulations! You've won the game!");
            printf(" Congratulations! You've won the game! ");
        }
    }
}
