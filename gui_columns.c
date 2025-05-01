#include "gui_columns.h"
#include "variables.h"
#include "card_images.h"

#define COLUMN_START_X 330
#define COLUMN_START_Y 120
#define CARD_WIDTH 60
#define CARD_HEIGHT 90
#define CARD_GAP_Y 30
#define COLUMN_GAP_X 10

void drawColumns(SDL_Renderer *renderer, TTF_Font *font, CardTextures *textures, int mouseX, int mouseY, int centered) {
    int startX = centered ? (1000 - (7 * (CARD_WIDTH + COLUMN_GAP_X) - COLUMN_GAP_X)) / 2 : 330;
    int startY = centered ? 40 : 120;

    for (int col = 0; col < 7; col++) {
        int x = startX + col * (CARD_WIDTH + COLUMN_GAP_X);
        int y = startY;
        Card *current = columns[col].top;

        while (current) {
            drawCardImage(renderer, font, textures, current, x, y, mouseX, mouseY);
            y += CARD_GAP_Y;
            current = current->next;
        }
    }

    if (!centered) {
        // Only show foundation pile layout when not centered (e.g., startup)
        for (int f = 0; f < 4; f++) {
            int x = 920;
            int y = 40 + f * (CARD_HEIGHT + 20);
            Card *top = foundations[f].top;
            if (top) {
                drawCardImage(renderer, font, textures, top, x, y, mouseX, mouseY);
            } else {
                SDL_Rect empty = {x, y, CARD_WIDTH, CARD_HEIGHT};
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawRect(renderer, &empty);
            }
        }
    }
}


