#ifndef GUI_CARDS_H
#define GUI_CARDS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "structures.h"

void drawCard(SDL_Renderer *renderer, TTF_Font *font, Card *card, int x, int y);

#endif
