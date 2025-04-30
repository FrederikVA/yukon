// gui_buttons.c
#include <SDL.h>
#include <SDL_ttf.h>

#include "gui.h"
#include "stateHandler.h"
#include <string.h>
#include <stdio.h>

#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 50
#define BUTTON_PADDING 20

typedef struct {
    SDL_Rect rect;
    const char *label;
    const char *command;
} Button;

static Button buttons[] = {
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "LOAD DECK", "LD"},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "SHOW DECK", "SW"},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "SHUFFLE (SPLIT)", "SI"},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "SHUFFLE (RANDOM)", "SR"},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "SAVE DECK", "SD"},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "PLAY GAME", "P"},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "HELP", "help"}
};

static const int buttonCount = sizeof(buttons) / sizeof(Button);

void drawButtons(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight) {
    int totalHeight = buttonCount * BUTTON_HEIGHT + (buttonCount - 1) * BUTTON_PADDING;
    int startY = (screenHeight - totalHeight) / 2;

    for (int i = 0; i < buttonCount; ++i) {
        Button *btn = &buttons[i];
        btn->rect.x = (screenWidth - BUTTON_WIDTH) / 2;
        btn->rect.y = startY + i * (BUTTON_HEIGHT + BUTTON_PADDING);

        // Draw button background
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
        SDL_RenderFillRect(renderer, &btn->rect);

        // Draw button border
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &btn->rect);

        // Render label text
        SDL_Color white = {255, 255, 255, 255}; // opaque white
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, btn->label, white);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        int textW, textH;
        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
        SDL_Rect textRect = {
            btn->rect.x + (BUTTON_WIDTH - textW) / 2,
            btn->rect.y + (BUTTON_HEIGHT - textH) / 2,
            textW, textH
        };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}

void handleButtonClick(int x, int y) {
    for (int i = 0; i < buttonCount; ++i) {
        Button *btn = &buttons[i];
        if (x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
            y >= btn->rect.y && y <= btn->rect.y + btn->rect.h) {
            handleCommand(btn->command);
            break;
        }
    }
}