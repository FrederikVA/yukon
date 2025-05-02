#include <stdio.h>
#include "move.h"
#include "stateHandler.h"
#include "gui_drag.h"
#include "variables.h"

DragState dragging = {0};

#define CARD_WIDTH 60
#define CARD_HEIGHT 90

void startDragFromPile(Card *card, int pileIndex, int isFoundation) {
    if (!card || !card->face_up) return;

    dragging.active = 1;
    dragging.startCard = card;
    dragging.fromColumnIndex = pileIndex;
    dragging.fromFoundation = isFoundation;  
    dragging.mouseX = 0;
    dragging.mouseY = 0;
}

void startDragFromColumn(Card *card, int columnIndex) {
    if (!card || !card->face_up) return;
    dragging.active = 1;
    dragging.startCard = card;
    dragging.fromColumnIndex = columnIndex;
    dragging.fromFoundation = 0;  
    dragging.mouseX = 0;
    dragging.mouseY = 0;
}

void drawDraggedCards(SDL_Renderer *r, TTF_Font *font, CardTextures *t, int x, int y) {
    if (!dragging.active || !dragging.startCard) return;

    Card *current = dragging.startCard;
    int offsetY = 0;
    while (current) {
        drawCardImage(r, font, t, current, x - 30, y + offsetY, 0);
        offsetY += 30;
        current = current->next;
    }
}

void stopDrag(int x, int y) {
    if (!dragging.active || !dragging.startCard) return;

    attemptDropAt(x, y);

    dragging.active = 0;
    dragging.startCard = NULL;
    dragging.fromColumnIndex = -1;
}

void attemptDropAt(int x, int y) {
    int targetCol = -1;
    int targetFoundation = -1;

    // Check columns (centered layout)
    int startX = (1000 - (7 * (CARD_WIDTH + 10) - 10)) / 2;
    int startY = 40;

    for (int i = 0; i < 7; i++) {
        int colX = startX + i * (CARD_WIDTH + 10);
        int y = startY;

        Card *curr = columns[i].top;
        int found = 0;
        while (curr) {
            SDL_Rect rect = {colX, y, CARD_WIDTH, CARD_HEIGHT};
            if (SDL_PointInRect(&(SDL_Point){x, y}, &rect)) {
                targetCol = i;
                found = 1;
                break;
            }
            y += 30;
            curr = curr->next;
        }

        if (!found && columns[i].top == NULL) {
            // If column is empty, allow drop in its placeholder
            SDL_Rect rect = {colX, y, CARD_WIDTH, CARD_HEIGHT};
            if (SDL_PointInRect(&(SDL_Point){x, y}, &rect)) {
                targetCol = i;
            }
        }

        if (targetCol != -1) break;
    }

    // 2. Check foundations (right side)
    if (targetCol == -1) {
        for (int i = 0; i < 4; i++) {
            int fx = 920;
            int fy = 40 + i * (CARD_HEIGHT + 20);
            SDL_Rect rect = {fx, fy, CARD_WIDTH, CARD_HEIGHT};
            if (SDL_PointInRect(&(SDL_Point){x, y}, &rect)) {
                targetFoundation = i;
                break;
            }
        }
    }

    // 3. Build command string and validate
    if (targetCol >= 0 || targetFoundation >= 0) {
        char cmd[20];
        char src[5], dst[5];

        if (dragging.fromFoundation) {
            sprintf(src, "F%d", dragging.fromColumnIndex + 1);
        } else {
            sprintf(src, "C%d", dragging.fromColumnIndex + 1);
        }

        if (dragging.startCard)
            sprintf(src + strlen(src), ":%c%c", dragging.startCard->rank, dragging.startCard->suit);

        if (targetCol >= 0)
            sprintf(dst, "C%d", targetCol + 1);
        else
            sprintf(dst, "F%d", targetFoundation + 1);

        snprintf(cmd, sizeof(cmd), "%s->%s", src, dst);

        printf("Trying move: %s\n", cmd);
        if (validateMoveInput(cmd) && validateMove()) {
            executeMove();
            strcpy(message, "Drag-drop move executed!");
        } else {
            strcpy(message, "Illegal move by drag-drop.");
        }
    } else {
        strcpy(message, "Dropped outside valid targets.");
    }
}
