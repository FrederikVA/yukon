#include <SDL2/SDL.h>
#include "gui.h"
#include "gui_buttons.h"

void runGUI() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Window *win = SDL_CreateWindow("Yukon GUI",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);
    if (!win) {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    // Simple event loop to keep the window open until closing it
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // drawing here later
        void drawButtons(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight);
        SDL_Delay(16);  // ~60 FPS frame cap
    }

    SDL_DestroyWindow(win);
    SDL_Quit();
}
