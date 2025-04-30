// gui.c
#include <SDL.h>
#include <SDL_ttf.h>
#include "gui.h"
#include "gui_buttons.h"
#include "stateHandler.h"

void runGUI() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0 || TTF_Init() != 0) {
        SDL_Log("Init Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Window *win = SDL_CreateWindow("Yukon GUI",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
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

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                handleButtonClick(e.button.x, e.button.y);
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // dark gray background
        SDL_RenderClear(renderer);

        drawButtons(renderer, font, 800, 600);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
}
