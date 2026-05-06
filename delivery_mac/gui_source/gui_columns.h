#ifndef GUI_COLUMNS_H
#define GUI_COLUMNS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "card_images.h"

Card *drawColumns(SDL_Renderer *renderer, TTF_Font *font, CardTextures *textures, int mouseX, int mouseY, int centered, int *outColumnIndex);

#endif
