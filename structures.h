#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct Card {
    char rank;     // 'A', '2', ..., 'T', 'J', 'Q', 'K'
    char suit;     // 'C', 'D', 'H', 'S'
    int face_up;   // 1 if face up, 0 if hidden
    struct Card *next;
} Card;

typedef struct {
    Card *top;     // Top card of a pile (column or foundation)
} Pile;

typedef enum {
    STARTUP,
    PLAY
} GamePhase;

typedef struct {
    int active;
    Card *startCard;
    int fromColumnIndex;
    int mouseX, mouseY;
} DragState;

#endif
