#ifndef CARD_IMAGES_H
#define CARD_IMAGES_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "structures.h"

typedef struct {
    SDL_Texture *cardback;
    SDL_Texture *heart;
    SDL_Texture *diamond;
    SDL_Texture *club;
    SDL_Texture *spade;

    SDL_Texture *rjack;
    SDL_Texture *bjack;
    SDL_Texture *rqueen;
    SDL_Texture *bqueen;
    SDL_Texture *rking;
    SDL_Texture *bking;
} CardTextures;

int loadCardTextures(SDL_Renderer *renderer, CardTextures *textures);
void freeCardTextures(CardTextures *textures);
SDL_Color getSuitColor(char suit);
void drawCardImage(SDL_Renderer *r, TTF_Font *font, CardTextures *t, Card *card, int x, int y, int highlight);

#endif
