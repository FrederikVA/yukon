#include "card_images.h"
#include <SDL_image.h>
#include <string.h>

#define CARD_WIDTH 60
#define CARD_HEIGHT 90

static SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) return NULL;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

int loadCardTextures(SDL_Renderer *renderer, CardTextures *t) {
    t->cardback = loadTexture(renderer, "assets/cardback.png");
    t->heart = loadTexture(renderer, "assets/heart.png");
    t->diamond = loadTexture(renderer, "assets/diamond.png");
    t->club = loadTexture(renderer, "assets/club.png");
    t->spade = loadTexture(renderer, "assets/spade.png");

    t->rjack = loadTexture(renderer, "assets/rjack.png");
    t->bjack = loadTexture(renderer, "assets/bjack.png");
    t->rqueen = loadTexture(renderer, "assets/rqueen.png");
    t->bqueen = loadTexture(renderer, "assets/bqueen.png");
    t->rking = loadTexture(renderer, "assets/rking.png");
    t->bking = loadTexture(renderer, "assets/bking.png");

    return t->cardback && t->heart && t->diamond && t->club && t->spade &&
           t->rjack && t->bjack && t->rqueen && t->bqueen && t->rking && t->bking;
}

void freeCardTextures(CardTextures *t) {
    SDL_DestroyTexture(t->cardback);
    SDL_DestroyTexture(t->heart);
    SDL_DestroyTexture(t->diamond);
    SDL_DestroyTexture(t->club);
    SDL_DestroyTexture(t->spade);
    SDL_DestroyTexture(t->rjack);
    SDL_DestroyTexture(t->bjack);
    SDL_DestroyTexture(t->rqueen);
    SDL_DestroyTexture(t->bqueen);
    SDL_DestroyTexture(t->rking);
    SDL_DestroyTexture(t->bking);
}

static SDL_Texture* getSuitTexture(CardTextures *t, char suit) {
    switch (suit) {
        case 'H': return t->heart;
        case 'D': return t->diamond;
        case 'C': return t->club;
        case 'S': return t->spade;
        default: return NULL;
    }
}

static SDL_Texture* getFaceTexture(CardTextures *t, char rank, char suit) {
    int red = (suit == 'H' || suit == 'D');
    switch (rank) {
        case 'J': return red ? t->rjack : t->bjack;
        case 'Q': return red ? t->rqueen : t->bqueen;
        case 'K': return red ? t->rking : t->bking;
        default: return NULL;
    }
}

SDL_Color getSuitColor(char suit) {
    switch (suit) {
        case 'H': return (SDL_Color){255, 0, 0, 255};
        case 'D': return (SDL_Color){255, 128, 0, 255};
        case 'C': return (SDL_Color){50, 100, 200, 255};
        case 'S': return (SDL_Color){0, 0, 0, 255};
        default: return (SDL_Color){30, 30, 30, 255};
    }
}

void drawCardImage(SDL_Renderer *r, TTF_Font *font, CardTextures *t, Card *card, int x, int y, int highlight) {
    SDL_Rect rect = {x, y, CARD_WIDTH, CARD_HEIGHT};

    if (!card->face_up) {
        SDL_RenderCopy(r, t->cardback, NULL, &rect);
        return;
    }

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderFillRect(r, &rect);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderDrawRect(r, &rect);

    char rank[3] = "";
    if (card->rank == 'T') {
        strcpy(rank, "10");
    } else {
        rank[0] = card->rank;
        rank[1] = '\0';
    }

    SDL_Color color = getSuitColor(card->suit);

    SDL_Texture *face = getFaceTexture(t, card->rank, card->suit);
    if (face) {
        SDL_Rect faceRect = {x, y, CARD_WIDTH, CARD_HEIGHT};
        SDL_RenderCopy(r, face, NULL, &faceRect);
    }

    SDL_Surface *surf = TTF_RenderText_Blended(font, rank, color);
    SDL_Texture *rankTex = SDL_CreateTextureFromSurface(r, surf);
    SDL_Rect rankRect = {x + 4, y + 3, surf->w, surf->h};
    SDL_RenderCopy(r, rankTex, NULL, &rankRect);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(rankTex);

    SDL_Texture *suitTex = getSuitTexture(t, card->suit);
    if (suitTex) {
        SDL_Rect iconRect = {x + CARD_WIDTH - 22, y + 4, 20, 20};
        SDL_RenderCopy(r, suitTex, NULL, &iconRect);
    }

    SDL_Surface *revSurf = TTF_RenderText_Blended(font, rank, color);
    SDL_Texture *revTex = SDL_CreateTextureFromSurface(r, revSurf);
    int rw = revSurf->w, rh = revSurf->h;
    SDL_Rect revRect = {x + CARD_WIDTH - rw - 4, y + CARD_HEIGHT - rh - 3, rw, rh};
    SDL_RenderCopyEx(r, revTex, NULL, &revRect, 180, NULL, SDL_FLIP_NONE);
    SDL_FreeSurface(revSurf);
    SDL_DestroyTexture(revTex);

    if (suitTex) {
        SDL_Rect suitRotated = {x + 4, y + CARD_HEIGHT - 24, 20, 20};
        SDL_RenderCopyEx(r, suitTex, NULL, &suitRotated, 180, NULL, SDL_FLIP_NONE);
    }

    if (highlight) {
        SDL_SetRenderDrawColor(r, 255, 255, 0, 255);
        SDL_RenderDrawRect(r, &rect);
    }
}
