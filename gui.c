// gui.c
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "gui.h"
#include "gui_buttons.h"
#include "stateHandler.h"
#include "gui_columns.h"
#include "card_images.h"
#include "variables.h"
#include "gui_drag.h"

#define CARD_WIDTH 60
#define CARD_HEIGHT 90

void runGUI() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0 || TTF_Init() != 0) {
        SDL_Log("Init Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Window *win = SDL_CreateWindow("Yukon GUI",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!win || !renderer) {
        SDL_Log("Window/Renderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 24);
    if (!font) {
        SDL_Log("Font Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return;
    }

    CardTextures cardTextures;
    if (!loadCardTextures(renderer, &cardTextures)) {
        SDL_Log("Failed to load card textures.\n");
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return;
    }

    SDL_Event e;
    int quit = 0;
    int mouseX = 0, mouseY = 0;

    while (!quit && running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
                running = 0;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (currentPhase == STARTUP) {
                    handleButtonClick(e.button.x, e.button.y);
                } else if (currentPhase == PLAY && e.button.button == SDL_BUTTON_LEFT) {
                    // Check if the exit button was clicked
                    handleExitButtonClick(e.button.x, e.button.y);
                
                    int colIndex = -1;
                    Card *hoveredCard = drawColumns(renderer, font, &cardTextures, mouseX, mouseY, currentPhase == PLAY, &colIndex);

                    // Check for drag from foundations (only top card)
                    for (int i = 0; i < 4; i++) {
                        Card *curr = foundations[i].top;
                        if (!curr) continue;
                        while (curr->next) curr = curr->next;  // last card

                        SDL_Rect rect = {920, 40 + i * (CARD_HEIGHT + 20), CARD_WIDTH, CARD_HEIGHT};
                        if (SDL_PointInRect(&(SDL_Point){e.button.x, e.button.y}, &rect)) {
                            startDragFromPile(curr, i, 1);  // from foundation
                            break;
                        }
                    }

                    if (hoveredCard) {
                        startDragFromColumn(hoveredCard, colIndex);
                    }
                }                
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                if (currentPhase == PLAY && e.button.button == SDL_BUTTON_LEFT) {
                    stopDrag(e.button.x, e.button.y);
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                mouseX = e.motion.x;
                mouseY = e.motion.y;
                if (dragging.active) {
                    dragging.mouseX = mouseX;
                    dragging.mouseY = mouseY;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // Only draw menu buttons in STARTUP mode
        if (currentPhase == STARTUP) {
            drawButtons(renderer, font, 300, 600);
        } else if (currentPhase == PLAY) {
            drawExitGameButton(renderer, font, 1000, 600);
        }

        int colIndex;
        Card *hoveredCard = drawColumns(renderer, font, &cardTextures, mouseX, mouseY, currentPhase == PLAY, &colIndex);

        if (currentPhase == PLAY && dragging.active) {
            drawDraggedCards(renderer, font, &cardTextures, dragging.mouseX, dragging.mouseY);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    freeCardTextures(&cardTextures);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
}
