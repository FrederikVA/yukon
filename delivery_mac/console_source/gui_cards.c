#include <string.h>
#include "gui_cards.h"

#define CARD_WIDTH 60
#define CARD_HEIGHT 90

//Just a fallback now 
void drawCard(SDL_Renderer *renderer, TTF_Font *font, Card *card, int x, int y) {
    SDL_Rect rect = {x, y, CARD_WIDTH, CARD_HEIGHT};

    if (!card->face_up) {
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);  // face-down
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &rect);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // face-up
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    char text[3] = {card->rank, card->suit, '\0'};

    SDL_Color color;
    if (card->suit == 'H' || card->suit == 'D') {
        color = (SDL_Color){220, 30, 30, 255};  // red
    } else {
        color = (SDL_Color){30, 30, 30, 255};   // black
    }

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int tw, th;
    SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
    SDL_Rect textRect = {x + (CARD_WIDTH - tw) / 2, y + (CARD_HEIGHT - th) / 2, tw, th};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
