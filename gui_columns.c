#include "gui_columns.h"
#include "variables.h"

#define COLUMN_START_X 330
#define COLUMN_START_Y 120
#define CARD_WIDTH 60
#define CARD_HEIGHT 90
#define CARD_GAP_Y 30
#define COLUMN_GAP_X 10

Card *drawColumns(SDL_Renderer *renderer, TTF_Font *font, CardTextures *textures, int mouseX, int mouseY, int centered, int *outColumnIndex) {
    int startX = centered ? (1000 - (7 * (CARD_WIDTH + COLUMN_GAP_X) - COLUMN_GAP_X)) / 2 : 330;
    int startY = centered ? 40 : 120;

    Card *hoveredCard = NULL;
    int hoveredCol = -1;

    for (int col = 0; col < 7; col++) {
        int x = startX + col * (CARD_WIDTH + COLUMN_GAP_X);
        int y = startY;

        // Draw placeholder behind the bottom card
        if (centered) {
            SDL_Rect empty = {x, y, CARD_WIDTH, CARD_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &empty);
        }

        Card *cards[100];
        int count = 0;
        Card *current = columns[col].top;
        while (current) {
            cards[count++] = current;
            current = current->next;
        }

        int hoveredIndex = -1;
        for (int i = 0; i < count; i++) {
            SDL_Rect rect = {x, startY + i * CARD_GAP_Y, CARD_WIDTH, CARD_HEIGHT};
            if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &rect)) {
                hoveredIndex = i;
                hoveredCard = cards[i];
                hoveredCol = col;
            }
        }

        y = startY;
        for (int i = 0; i < count; i++) {
            int highlight = (i == hoveredIndex);
            drawCardImage(renderer, font, textures, cards[i], x, y, highlight);
            y += CARD_GAP_Y;
        }
    }

    if (centered) {
        for (int f = 0; f < 4; f++) {
            int x = 920;
            int y = 40 + f * (CARD_HEIGHT + 20);
            
            Card *current = foundations[f].top;
            while (current && current->next) {
                current = current->next;
            }
            if (current) {
                drawCardImage(renderer, font, textures, current, x, y, 0);
            }
             else {
                SDL_Rect empty = {x, y, CARD_WIDTH, CARD_HEIGHT};
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawRect(renderer, &empty);
            }
        }
    }

    if (outColumnIndex) *outColumnIndex = hoveredCol;
    return hoveredCard;
}
