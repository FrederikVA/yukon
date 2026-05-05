#include <SDL.h>
#include <SDL_ttf.h>
#include <dirent.h>
#include <time.h>
#include "gui.h"
#include "stateHandler.h"
#include "variables.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>

#define BUTTON_WIDTH 250
#define BUTTON_HEIGHT 50
#define BUTTON_PADDING 20
#define NARROW_BUTTON_WIDTH 180 
#define PLAY_BUTTON_WIDTH 130
#define PLAY_BUTTON_HEIGHT 36
#define SAVE_LIST_MAX 6
#define STARTUP_BUTTON_CENTER_X 710

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
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "SAVE DECK", "SD", 0},
    {{0, 0, BUTTON_WIDTH, BUTTON_HEIGHT}, "PLAY GAME", "P", 0},
    {{0, 0, NARROW_BUTTON_WIDTH, BUTTON_HEIGHT}, "QUIT", "QQ", 1}
};

static const int buttonCount = sizeof(buttons) / sizeof(Button);

static SDL_Rect undoButton;
static SDL_Rect redoButton;
static SDL_Rect saveButton;
static SDL_Rect exitButtonRect;
static SDL_Rect savedStateRects[SAVE_LIST_MAX];
static char savedStateNames[SAVE_LIST_MAX][100];
static int savedStateCount = 0;

static void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

static void drawSmallButton(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect *rect, const char *label, int red) {
    if (red) {
        SDL_SetRenderDrawColor(renderer, 190, 45, 45, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 65, 105, 175, 255);
    }
    SDL_RenderFillRect(renderer, rect);
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderDrawRect(renderer, rect);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, label, white);
    if (!textSurface) return;
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect textRect = {
        rect->x + (rect->w - textSurface->w) / 2,
        rect->y + (rect->h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

static void shortenText(const char *source, char *dest, int maxChars) {
    int len = (int)strlen(source);

    if (len <= maxChars) {
        snprintf(dest, maxChars + 1, "%s", source);
        return;
    }

    if (maxChars <= 3) {
        snprintf(dest, maxChars + 1, "%s", source);
        return;
    }

    int prefix = maxChars - 3;
    for (int i = 0; i < prefix; i++) {
        dest[i] = source[i];
    }
    dest[prefix] = '.';
    dest[prefix + 1] = '.';
    dest[prefix + 2] = '.';
    dest[prefix + 3] = '\0';
}

static void drawSavedGameName(SDL_Renderer *renderer, TTF_Font *font, const char *name, int x, int y) {
    char displayName[24];
    SDL_Color white = {245, 245, 245, 255};

    shortenText(name, displayName, 18);
    drawText(renderer, font, displayName, x, y, white);
}

static void refreshSavedStateList(void) {
    struct dirent *entry;
    DIR *dp = opendir(".");
    savedStateCount = 0;

    if (!dp) return;
    while ((entry = readdir(dp)) != NULL && savedStateCount < SAVE_LIST_MAX) {
        int len = (int)strlen(entry->d_name);
        if (len > 4 && strcmp(entry->d_name + len - 4, ".txt") == 0 &&
            strcmp(entry->d_name, "best_time.txt") != 0) {
            FILE *f = fopen(entry->d_name, "r");
            char marker[20] = "";
            int version = 0;
            int isStateFile = f && fscanf(f, "%19s %d", marker, &version) == 2 &&
                              strcmp(marker, "YUKON_STATE") == 0 && version == 1;
            if (f) fclose(f);

            if (isStateFile) {
                snprintf(savedStateNames[savedStateCount], sizeof(savedStateNames[savedStateCount]), "%s", entry->d_name);
                savedStateCount++;
            }
        }
    }
    closedir(dp);
}

void drawButtons(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight) {
    (void)screenWidth;
    int totalHeight = buttonCount * BUTTON_HEIGHT + (buttonCount - 1) * BUTTON_PADDING;
    int startY = (screenHeight - totalHeight) / 2;

    for (int i = 0; i < buttonCount; ++i) {
        Button *btn = &buttons[i];

        int btnWidth = btn->isNarrow ? NARROW_BUTTON_WIDTH : BUTTON_WIDTH;
        btn->rect.w = btnWidth;
        btn->rect.h = BUTTON_HEIGHT;
        btn->rect.x = STARTUP_BUTTON_CENTER_X - btnWidth / 2;
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

void drawPlayControls(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight) {
    int panelX = screenWidth - 260;
    int y = 32;
    char text[200];
    SDL_Color white = {245, 245, 245, 255};
    SDL_Color muted = {190, 190, 190, 255};

    SDL_Rect panel = {panelX - 10, 20, 240, screenHeight - 40};
    SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &panel);

    drawText(renderer, font, "GAME", panelX, y, white);
    y += 42;

    snprintf(text, sizeof(text), "Time: %ld sec", getGameElapsed());
    drawText(renderer, font, text, panelX, y, white);
    y += 34;

    if (bestCompletionTime > 0) {
        snprintf(text, sizeof(text), "Best: %ld sec", bestCompletionTime);
    } else {
        snprintf(text, sizeof(text), "Best: --");
    }
    drawText(renderer, font, text, panelX, y, white);
    y += 42;

    undoButton = (SDL_Rect){panelX, y, PLAY_BUTTON_WIDTH, PLAY_BUTTON_HEIGHT};
    drawSmallButton(renderer, font, &undoButton, "UNDO", 0);
    y += PLAY_BUTTON_HEIGHT + 10;

    redoButton = (SDL_Rect){panelX, y, PLAY_BUTTON_WIDTH, PLAY_BUTTON_HEIGHT};
    drawSmallButton(renderer, font, &redoButton, "REDO", 0);
    y += PLAY_BUTTON_HEIGHT + 10;

    saveButton = (SDL_Rect){panelX, y, PLAY_BUTTON_WIDTH, PLAY_BUTTON_HEIGHT};
    drawSmallButton(renderer, font, &saveButton, "SAVE", 0);
    y += PLAY_BUTTON_HEIGHT + 18;

    exitButtonRect = (SDL_Rect){panelX, y, PLAY_BUTTON_WIDTH, PLAY_BUTTON_HEIGHT};
    drawSmallButton(renderer, font, &exitButtonRect, "EXIT", 1);
    y += PLAY_BUTTON_HEIGHT + 28;

    drawText(renderer, font, "SAVED GAMES", panelX, y, white);
    y += 34;

    refreshSavedStateList();
    for (int i = 0; i < savedStateCount; i++) {
        savedStateRects[i] = (SDL_Rect){panelX, y, 200, 30};
        SDL_SetRenderDrawColor(renderer, 65, 65, 65, 255);
        SDL_RenderFillRect(renderer, &savedStateRects[i]);
        SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
        SDL_RenderDrawRect(renderer, &savedStateRects[i]);
        drawSavedGameName(renderer, font, savedStateNames[i], panelX + 8, y + 2);
        y += 36;
    }

    if (savedStateCount == 0) {
        drawText(renderer, font, "No saved games", panelX, y, muted);
        y += 34;
    }

    y += 12;
    drawText(renderer, font, "Message:", panelX, y, muted);
    y += 28;
    drawText(renderer, font, message, panelX, y, white);
}

void drawStartupSavedGames(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight) {
    int panelX = screenWidth - 300;
    int y = 115;
    SDL_Color white = {245, 245, 245, 255};
    SDL_Color muted = {190, 190, 190, 255};

    SDL_Rect panel = {panelX - 10, y - 12, 250, screenHeight - 230};
    SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &panel);

    drawText(renderer, font, "SAVED GAMES", panelX, y, white);
    y += 42;

    refreshSavedStateList();
    for (int i = 0; i < savedStateCount; i++) {
        savedStateRects[i] = (SDL_Rect){panelX, y, 210, 32};
        SDL_SetRenderDrawColor(renderer, 65, 65, 65, 255);
        SDL_RenderFillRect(renderer, &savedStateRects[i]);
        SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
        SDL_RenderDrawRect(renderer, &savedStateRects[i]);
        drawSavedGameName(renderer, font, savedStateNames[i], panelX + 8, y + 3);
        y += 38;
    }

    if (savedStateCount == 0) {
        drawText(renderer, font, "No saved games", panelX, y, muted);
    }
}

static void buildUniqueGuiSaveCommand(char *command, int commandSize) {
    char filename[80];
    long now = (long)time(NULL);

    for (int i = 0; i < 100; i++) {
        if (i == 0) {
            snprintf(filename, sizeof(filename), "gui_save_%ld.txt", now);
        } else {
            snprintf(filename, sizeof(filename), "gui_save_%ld_%d.txt", now, i);
        }

        FILE *f = fopen(filename, "r");
        if (!f) {
            snprintf(command, commandSize, "S %s", filename);
            return;
        }
        fclose(f);
    }

    snprintf(command, commandSize, "S gui_save_%ld_fallback.txt", now);
}

int handleSavedGameClick(int x, int y) {
    SDL_Point point = {x, y};
    char command[120];

    for (int i = 0; i < savedStateCount; i++) {
        if (SDL_PointInRect(&point, &savedStateRects[i])) {
            snprintf(command, sizeof(command), "L %s", savedStateNames[i]);
            handleCommand(command);
            return 1;
        }
    }

    return 0;
}

int handlePlayControlClick(int x, int y) {
    SDL_Point point = {x, y};

    if (SDL_PointInRect(&point, &undoButton)) {
        handleCommand("U");
        return 1;
    }
    if (SDL_PointInRect(&point, &redoButton)) {
        handleCommand("R");
        return 1;
    }
    if (SDL_PointInRect(&point, &saveButton)) {
        char command[120];
        buildUniqueGuiSaveCommand(command, sizeof(command));
        handleCommand(command);
        return 1;
    }
    if (SDL_PointInRect(&point, &exitButtonRect)) {
        handleCommand("Q");
        return 1;
    }

    return handleSavedGameClick(x, y);
}
