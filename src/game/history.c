#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "variables.h"
#include "fileHandler.h"
#include "history.h"

typedef struct HistoryNode {
    Pile columns[7];
    Pile foundations[4];
    struct HistoryNode *next;
} HistoryNode;

static HistoryNode *undoStack = NULL;
static HistoryNode *redoStack = NULL;

// Undo/redo needs deep copies because piles are mutable singly linked lists.
static Card *copyCardList(Card *source) {
    Card *copyHead = NULL;
    Card *copyTail = NULL;

    while (source != NULL) {
        Card *copy = (Card *)malloc(sizeof(Card));
        if (!copy) {
            printf("Memory allocation failed!\n");
            exit(1);
        }

        copy->rank = source->rank;
        copy->suit = source->suit;
        copy->face_up = source->face_up;
        copy->next = NULL;

        if (copyHead == NULL) {
            copyHead = copy;
        } else {
            copyTail->next = copy;
        }
        copyTail = copy;
        source = source->next;
    }

    return copyHead;
}

static void freeCardList(Card *card) {
    while (card != NULL) {
        Card *next = card->next;
        free(card);
        card = next;
    }
}

static void freeSnapshot(HistoryNode *node) {
    if (!node) return;

    for (int i = 0; i < 7; i++) {
        freeCardList(node->columns[i].top);
    }
    for (int i = 0; i < 4; i++) {
        freeCardList(node->foundations[i].top);
    }
    free(node);
}

static void clearStack(HistoryNode **stack) {
    while (*stack != NULL) {
        HistoryNode *next = (*stack)->next;
        freeSnapshot(*stack);
        *stack = next;
    }
}

// Stores a full board snapshot so undo/redo restores columns, foundations, deck, phase, and timer together.
static HistoryNode *createSnapshot(void) {
    HistoryNode *node = (HistoryNode *)malloc(sizeof(HistoryNode));
    if (!node) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    for (int i = 0; i < 7; i++) {
        node->columns[i].top = copyCardList(columns[i].top);
    }
    for (int i = 0; i < 4; i++) {
        node->foundations[i].top = copyCardList(foundations[i].top);
    }
    node->next = NULL;
    return node;
}

static void pushSnapshot(HistoryNode **stack, HistoryNode *node) {
    node->next = *stack;
    *stack = node;
}

static HistoryNode *popSnapshot(HistoryNode **stack) {
    HistoryNode *node = *stack;
    if (node != NULL) {
        *stack = node->next;
        node->next = NULL;
    }
    return node;
}

static void restoreSnapshot(HistoryNode *node) {
    clearColumns();
    clearFoundations();

    for (int i = 0; i < 7; i++) {
        columns[i].top = copyCardList(node->columns[i].top);
    }
    for (int i = 0; i < 4; i++) {
        foundations[i].top = copyCardList(node->foundations[i].top);
    }
}

// Called immediately before executing a valid move so U can return to the previous board state.
void recordUndoState(void) {
    if (currentPhase != PLAY) {
        return;
    }

    pushSnapshot(&undoStack, createSnapshot());
    clearStack(&redoStack);
}

// Implements U by moving the current snapshot to the redo stack and restoring the previous snapshot.
int undoMove(void) {
    HistoryNode *previous = popSnapshot(&undoStack);
    if (!previous) {
        return 0;
    }

    pushSnapshot(&redoStack, createSnapshot());
    restoreSnapshot(previous);
    freeSnapshot(previous);
    return 1;
}

// Implements R only after undo by restoring the latest snapshot from the redo stack.
int redoMove(void) {
    HistoryNode *next = popSnapshot(&redoStack);
    if (!next) {
        return 0;
    }

    pushSnapshot(&undoStack, createSnapshot());
    restoreSnapshot(next);
    freeSnapshot(next);
    return 1;
}

void clearHistory(void) {
    clearStack(&undoStack);
    clearStack(&redoStack);
}
