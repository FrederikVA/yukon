#include <SDL.h>
#include <SDL_ttf.h>
#include "gui.h"
#include "stateHandler.h"
#include <string.h>
#include <stdio.h>

#define BUTTON_WIDTH 250
#define BUTTON_HEIGHT 50
#define BUTTON_PADDING 20
#define NARROW_BUTTON_WIDTH 180 

typedef struct {
    SDL_Rect rect;
    const char *label;
    const char *command;
    int isNarrow;
} Button;

static Button buttons[] = {
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "LOAD DECK", "LD", 0},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "SHOW DECK", "SW", 0},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "SPLIT-SHUFFLE", "SI", 0},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "RANDOM-SHUFFLE", "SR", 0},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "SAVED DECKS", "SD", 0},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "PLAY GAME", "P", 0},
    {{0, 0, NARROW_BUTTON_WIDTH, BUTTON_HEIGHT}, "QUIT", "QQ", 1}
};

static const int buttonCount = sizeof(buttons) / sizeof(Button);

void drawButtons(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight) {
    int totalHeight = buttonCount * BUTTON_HEIGHT + (buttonCount - 1) * BUTTON_PADDING;
    int startY = (screenHeight - totalHeight) / 2;

    for (int i = 0; i < buttonCount; ++i) {
        Button *btn = &buttons[i];

        int btnWidth = btn->isNarrow ? NARROW_BUTTON_WIDTH : BUTTON_WIDTH;
        btn->rect.w = btnWidth;
        btn->rect.h = BUTTON_HEIGHT;
        btn->rect.x = (screenWidth - btnWidth) / 2;
        btn->rect.y = startY + i * (BUTTON_HEIGHT + BUTTON_PADDING);

        // Background
        if (btn->isNarrow) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
        }
        SDL_RenderFillRect(renderer, &btn->rect);

        // Border
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &btn->rect);

        // Label
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, btn->label, white);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        int textW, textH;
        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
        SDL_Rect textRect = {
            btn->rect.x + (btn->rect.w - textW) / 2,
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

void drawExitGameButton(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight) {
    SDL_Rect exitButton = {
        screenWidth - NARROW_BUTTON_WIDTH - 20,
        screenHeight - BUTTON_HEIGHT - 20,
        NARROW_BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    // Background
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &exitButton);

    // Border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &exitButton);

    // Label
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, "EXIT", white);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textW, textH;
    SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
    SDL_Rect textRect = {
        exitButton.x + (exitButton.w - textW) / 2,
        exitButton.y + (BUTTON_HEIGHT - textH) / 2,
        textW, textH
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void handleExitButtonClick(int x, int y) {
    SDL_Rect exitButton = {
        1000 - NARROW_BUTTON_WIDTH - 20,
        600 - BUTTON_HEIGHT - 20,
        NARROW_BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    if (x >= exitButton.x && x <= exitButton.x + exitButton.w &&
        y >= exitButton.y && y <= exitButton.y + exitButton.h) {
        handleCommand("Q");  // Back to startup phase
    }
}
