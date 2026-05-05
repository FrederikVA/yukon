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
#include "move.h"
#include "timer.h"

#define CARD_WIDTH 60
#define CARD_HEIGHT 90
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 700

void runGUI() {
    loadBestCompletionTime();

    if (SDL_Init(SDL_INIT_VIDEO) != 0 || TTF_Init() != 0) {
        SDL_Log("Init Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Window *win = SDL_CreateWindow("Yukon GUI",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                    if (!handleSavedGameClick(e.button.x, e.button.y)) {
                        handleButtonClick(e.button.x, e.button.y);
                    }
                } else if (currentPhase == PLAY && e.button.button == SDL_BUTTON_LEFT) {
                    if (handlePlayControlClick(e.button.x, e.button.y)) {
                        continue;
                    }
                
                    int colIndex = -1;
                    Card *hoveredCard = drawColumns(renderer, font, &cardTextures, mouseX, mouseY, currentPhase == PLAY, &colIndex);

                    // Check for drag from foundations (only top card)
                    for (int i = 0; i < 4; i++) {
                        Card *curr = foundations[i].top;
                        if (!curr) continue;
                        while (curr->next) curr = curr->next;  // last card

                        SDL_Rect rect = {760, 40 + i * (CARD_HEIGHT + 20), CARD_WIDTH, CARD_HEIGHT};
                        if (SDL_PointInRect(&(SDL_Point){e.button.x, e.button.y}, &rect)) {
                            startDragFromPile(curr, i, 1);  // from foundation
                            dragging.mouseX = e.button.x;
                            dragging.mouseY = e.button.y;
                            break;
                        }
                    }

                    if (e.button.clicks == 2 && hoveredCard) {
                        int movedToFoundation = 0;
                        // Make sure it's the last card in its column
                        Card *curr = columns[colIndex].top;
                        while (curr && curr->next) {
                            curr = curr->next;
                        }
                    
                        if (curr == hoveredCard) {
                            char src[10], cmd[20];
                            snprintf(src, sizeof(src), "C%d:%c%c", colIndex + 1, hoveredCard->rank, hoveredCard->suit);
                    
                            for (int f = 0; f < 4; f++) {
                                snprintf(cmd, sizeof(cmd), "%s->F%d", src, f + 1);
                                if (validateMoveInput(cmd) && validateMove()) {
                                    executeMove();
                                    strcpy(message, "Card moved to foundation!");
                                    cancelActiveDrag();
                                    movedToFoundation = 1;
                                    break;
                                }
                            }
                        }
                        if (movedToFoundation) {
                            continue;
                        }
                    }                                      

                    if (hoveredCard) {
                        startDragFromColumn(hoveredCard, colIndex);
                        dragging.mouseX = e.button.x;
                        dragging.mouseY = e.button.y;
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

        int colIndex;
        drawColumns(renderer, font, &cardTextures, mouseX, mouseY, currentPhase == PLAY, &colIndex);

        // Draw controls after cards so UI stays above the tableau.
        if (currentPhase == STARTUP) {
            drawButtons(renderer, font, SCREEN_WIDTH, SCREEN_HEIGHT);
            drawStartupSavedGames(renderer, font, SCREEN_WIDTH, SCREEN_HEIGHT);
        } else if (currentPhase == PLAY) {
            drawPlayControls(renderer, font, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

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
