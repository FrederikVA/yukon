#ifndef GUI_BUTTONS_H
#define GUI_BUTTONS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void drawButtons(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight);
void handleButtonClick(int x, int y);

#endif
